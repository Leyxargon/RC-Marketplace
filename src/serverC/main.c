#include "serverC.h"

int main(int argc, char **argv) {
	int opt, portM = 8000, portC = 8002;
	char address[200];
	address[0] = '\0';
	if (argc < 3 || argc > 7)
		printUsage(argv[0]);
	else {
		while ((opt = getopt(argc, argv, "i:p:m:")) != -1) {
			switch (opt) {
				case 'i':
					strcpy(address, optarg);
					break;
				case 'p':
					if ((portC = atoi(optarg)) <= 0)
						printUsage(argv[0]);
					break;
				case 'm':
					if ((portM = atoi(optarg)) <= 0)
						printUsage(argv[0]);
					break;
				case ':':
				case '?':
					printUsage(argv[0]);
			}
		}
		if (address[0] == '\0')
			printUsage(argv[0]);
	}

	/* VARIABILI SERVER verso clientN */
	int listenfd, connfd, sockfd;
	/* 	listenfd: descrittore di ascolto
	 *	connfd: descrittore di connessione
	 *	sockfd: descrittore di servizio
	 */
	struct sockaddr_in s_servaddr, s_cliaddr;
	int len = sizeof(s_cliaddr);
	/* 	servaddr: informazioni indirizzo server
	 *	cliaddr: informazioni indirizzo client connesso
	 */
	int option = 1, i, maxi = -1, maxd, ready, client[FD_SETSIZE];
	/*	option: flag per SetSockOpt
	 *	maxi: indice massimo
	 *	maxd: descrittore in uso massimo
	 *	ready: numero di descrittori pronti
	 *	client: array di client connessi
	 */
	fd_set active_fd_set, read_fd_set;	/* insiemi di descrittori */
	char iplog[FD_SETSIZE][16];	/* array di indirizzi IP usato per il logging */
	int portlog[FD_SETSIZE];	/* array di porte usato per il logging */

	/* VARIABILI CLIENT verso serverM */
	struct sockaddr_in c_servaddr;
	int masterfd;

	/* VARIABILI LIVELLO APPLICAZIONE */
	char buf[BUFSIZE];
	pct_c richiesta;

	/* CLIENT verso serverM */
	/* creazione socket */
	masterfd = Socket(AF_INET, SOCK_STREAM, 0);
	c_servaddr.sin_family = AF_INET;
	c_servaddr.sin_port = htons(portM);

	/* lettura indirizzo IP */
	struct hostent *c_cliaddr;
	char *addr, **alias;
	c_cliaddr = GetHostByName(address);
    alias = c_cliaddr -> h_addr_list;	/* legge gli indirizzi IP dall'eventuale indirizzo simbolico */
	addr = (char *) inet_ntop(c_cliaddr -> h_addrtype, *alias, buf, sizeof(buf));
	Inet_pton(AF_INET, addr, &c_servaddr.sin_addr);

	/* connessione al server */
	Connect(masterfd, &c_servaddr);

	fputs("Connessione al serverM\n", stdout);
	Write(masterfd, "C", 1);	/* identificazione al server */
	Read(masterfd, buf, 1);	/* leggi esito connessione */
	if (buf[0] == '0')
		printf("Connesso al serverM\n");
	else {
		fputs("Connessione fallita.\n", stderr);
		exit(1);
	}

	/* SERVER verso client */
	/* creazione socket */
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	/* impostazioni socket */
	SetSockOpt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	/* indirizzo del server */
	s_servaddr.sin_family = AF_INET;
	s_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_servaddr.sin_port = htons(portC);

	fprintf(stdout, "Indirizzo IP: %s\n", inet_ntoa(s_servaddr.sin_addr));
	fprintf(stdout, "Porta: %d\n", (int) ntohs(s_servaddr.sin_port));

	/* assegnazione indirizzo */
	Bind(listenfd, &s_servaddr);
	/* messa in ascolto */
	Listen(listenfd, 1024);

	maxd = listenfd;

	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1; /* inizializzazione vettore di client */

	FD_ZERO(&active_fd_set); /* inizializzazione a zero l'insieme dei descrittori */
	FD_SET(listenfd, &active_fd_set); /* aggiunta del descrittore di ascolto */
	FD_SET(masterfd, &active_fd_set); /* aggiunta del descrittore associato al serverM */

	/* esecuzione server */
	while (1) {
		read_fd_set = active_fd_set; /* imposta il set di descrittori per la lettura */
		ready = Select(maxd + 1, &read_fd_set, NULL, NULL, NULL);

		/* Se è arrivata una richiesta di connessione, il socket di ascolto
		è leggibile: viene invocata accept() e creato un socket di connessione */
		if (FD_ISSET(listenfd, &read_fd_set)) {
			/* accettazione richiesta di connessione */
			connfd = Accept(listenfd, &s_cliaddr, &len);
			Read(connfd, buf, 1);
			if (buf[0] == 'c') {
				/* identificazione client */
				Write(connfd, "0", 1);
				for (i = 0; i < FD_SETSIZE; i++)
					if (client[i] < 0) {
						client[i] = connfd;
						break;
					}
				/* errore se non ci sono posti liberi nel vettore client */
				if (i == FD_SETSIZE) {
					fputs("accept: client array size full\n", stderr);
					exit(1);
				}
				inet_ntop(AF_INET, &s_cliaddr.sin_addr, iplog[i], sizeof(iplog[i]));	/* memorizza l'indirizzo del client per il logging */
				portlog[i] = ntohs(s_cliaddr.sin_port);	/* memorizza la porta del client per il logging */
				fprintf(stdout, "(%s:%d) Client connesso.\n", iplog[i], portlog[i]);
				/* inserisce connfd tra i descrittori da controllare ed aggiorna maxd */
				FD_SET(connfd, &active_fd_set);
				if (connfd > maxd)
					maxd = connfd;
				if (i > maxi)
					maxi = i;
			}
			else {
				Write(connfd, "1", 1);
				close(connfd);
				fputs("Connessione client rifiutata.\n", stderr);
			}
			--ready; /* richiesta accettata/rifiutata, un socket in meno */
		}

		/* controlla i socket attivi per controllare se sono leggibili */
		for (i = 0; i <= maxi && ready > 0; i++) {
			if ((sockfd = client[i]) >= 0) { /* se il descrittore non è stato selezionato, viene saltato */
				if (FD_ISSET(sockfd, &read_fd_set)) {
					/* leggi da sockfd */
					if (!Read(sockfd, &richiesta, sizeof(richiesta)))
						elaboraRichiesta(masterfd, sockfd, &richiesta);
					else {
						/* chiusura connessione da parte del client */
						fprintf(stdout, "(%s:%d) Connessione chiusa da client.\n", iplog[i], portlog[i]);
						FD_CLR(sockfd, &active_fd_set); /* rimuove sockfd dalla lista dei socket da controllare */
						client[i] = -1; /* cancella sockfd da client */
					}
					--ready; /* richiesta elaborata o connessione chiusa, un socket in meno */
				}
			}
		}
	}

	exit(0);
}
