#include "liste/lista.h"
#include "wrapper.h"
#include "liste/iofile.h"
#include "pct_r.h"
#include "pct_l.h"

int main(int argc, char **argv) {
	/* gestione errore */
	if (argc != 2) {
		fprintf(stderr,"uso: %s <IPaddress>\n",argv[0]);
		exit(1);
	}

	/* VARIABILI SERVER verso clientN */
	int listenfd, connfd, sockfd;
	/* 	listenfd: descrittore di ascolto
	 *	connfd: descrittore di connessione
	 *	sockfd: descrittore di servizio
	 */
	int len;
	struct sockaddr_in s_servaddr, s_cliaddr;
	/* 	s_servaddr: informazioni indirizzo server
	 *	s_cliaddr: informazioni indirizzo client connesso
	 */
	int option = 1, i, maxi = -1, maxd, ready, client[FD_SETSIZE];
	fd_set active_fd_set, read_fd_set;	/* insiemi di descrittori */

	/* VARIABILI CLIENT verso serverM */
	struct sockaddr_in c_servaddr;
	struct hostent *c_cliaddr;
	int masterfd;

	char **alias;
	char *addr;

	/* VARIABILI LIVELLO APPLICAZIONE */
	Utente *u_tmp = NULL;
	Negozio *n_tmp = NULL;
	Prodotto *p_tmp = NULL;
	char buf[BUFSIZE];
	pct_r richiesta;
	pct_l login;


	/* CLIENT verso serverM */
	/* creazione socket */
	masterfd = Socket(AF_INET, SOCK_STREAM, 0);
	c_servaddr.sin_family = AF_INET;
	c_servaddr.sin_port = htons(8000);

	/* lettura indirizzo IP */
	c_cliaddr = GetHostByName(argv[1]);
    alias = c_cliaddr -> h_addr_list;	/* legge gli indirizzi IP dall'eventuale indirizzo simbolico */
	addr = (char *) inet_ntop(c_cliaddr -> h_addrtype, *alias, buf, sizeof(buf));
	Inet_pton(AF_INET, addr, &c_servaddr.sin_addr);

	/* connessione al server */
	Connect(masterfd, &c_servaddr);

	fputs("Connessione al serverM\n", stdout);
	Write(masterfd, "N", 1);	/* identificazione al server */
	Read(masterfd, buf, 1);	/* leggi esito connessione */
	if (!strncmp(buf, "0", 1))
		printf("Connesso al serverM\n");
	else {
		fputs("Connessione fallita\n", stderr);
		exit(1);
	}
	
	/* SERVER verso clientN */
	/* creazione socket */
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	/* impostazioni socket */
	SetSockOpt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	/* indirizzo del server */
	s_servaddr.sin_family = AF_INET;
	s_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_servaddr.sin_port = htons(8001);
	
	fprintf(stdout, "Indirizzo IP: %s\n", inet_ntoa(s_servaddr.sin_addr));
	fprintf(stdout, "Porta: %d\n", (int) ntohs(s_servaddr.sin_port));
	
	/* assegnazione indirizzo */
	Bind(listenfd, &s_servaddr);
	/* messa in ascolto */
	Listen(listenfd, 1024);
	
	maxd = masterfd > listenfd ? masterfd : listenfd; /* maxd è il valore massimo dei descrittori in uso */
	
	/* il vettore client contiene i descrittori dei socket connessi */
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1; /* inizializza il vettore di client */
	
	FD_ZERO(&active_fd_set); /* inizializza a zero l'insieme dei descrittori */
	FD_SET(listenfd, &active_fd_set); /* aggiunge il descrittore di ascolto */
	FD_SET(masterfd, &active_fd_set); /* aggiunge il descrittore del serverM */
	
	/* esecuzione server */
	while (1) {
		read_fd_set = active_fd_set; /* imposta il set di descrittori per la lettura */
		ready = Select(maxd + 1, &read_fd_set, NULL, NULL, NULL);
		
		/* Se è arrivata una richiesta di connessione, il socket di ascolto
		è leggibile: viene invocata accept() e creato un socket di connessione */
		if (FD_ISSET(listenfd, &read_fd_set)) {
			/* accettazione richiesta di connessione */
			connfd = Accept(listenfd, &s_cliaddr, &len);
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
			fputs("Client connesso.\n", stdout);
			/* inserisce connfd tra i descrittori da controllare ed aggiorna maxd */
			FD_SET(connfd, &active_fd_set);
			if (connfd > maxd)
				maxd = connfd;
			if (i > maxi)
				maxi = i;
			--ready; /* richiesta accettata/rifiutata, un socket in meno */
		}
		
		/* controlla i socket attivi per controllare se sono leggibili */
		for (i = 0; i <= maxi && ready > 0; i++) {
			if ((sockfd = client[i]) >= 0) { /* se il descrittore non è stato selezionato, viene saltato */
				if (FD_ISSET(sockfd, &read_fd_set)) {
					/* leggi da sockfd */
					if (!Read(sockfd, &richiesta, sizeof(richiesta))) {
						richiesta.type = 'N';
						switch (richiesta.rich) {
							case '5':
								Write(masterfd, &richiesta, sizeof(richiesta));
								do {
									Read(masterfd, &buf, sizeof(buf));
									Write(sockfd, &buf, sizeof(buf));
								} while (buf[0] != '\0');
								
								break;
							case '6':
								Write(masterfd, &richiesta, sizeof(richiesta));
								do {
									Read(masterfd, &buf, sizeof(buf));
									Write(sockfd, &buf, sizeof(buf));
								} while (buf[0] != '\0');
								
								break;
							case '9':	/* RICHIESTA DI LOGIN */
								Write(sockfd, "0", 1);	/* invio pacchetto di conferma al client */
								Read(sockfd, &login, sizeof(login));	/* ricezione pacchetto di login dal client */
								Write(masterfd, &richiesta, sizeof(richiesta));	/* invio pacchetto di richiesta al serverM */
								Read(masterfd, &buf, 1);		/* lettura esito richiesta dal serverM */
								Write(masterfd, &login, sizeof(login));	/* invio pacchetto di login al server M */
								Read(masterfd, &buf, 1);
								Write(sockfd, &buf, 1);
								
								break;
							default:
								Write(masterfd, &richiesta, sizeof(richiesta));
								Read(masterfd, buf, 1);
								if (buf[0] == '0') {
									fputs("Operazione effettuata con successo.\n", stdout);
									Write(sockfd, "0", 1);
								}
								else {
									fputs("Operazione fallita.\n", stdout);
									Write(sockfd, "1", 1);
								}
								
								break;
						}
					}
					else {
						/* chiusura connessione da parte del client */
						fputs("Connessione chiusa da client.\n", stdout);
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
