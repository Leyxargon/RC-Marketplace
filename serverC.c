#include "liste/lista.h"
#include "wrapper.h"
#include "liste/iofile.h"
#include "pct_c.h"
#include <time.h>
#include <string.h>

int main(int argc, char **argv) {
	/* gestione errore */
	if (argc != 2) {
		fprintf(stderr,"usage: %s <IPaddress>\n",argv[0]);
		exit(1);
	}
	
	
	pct_c richiesta;
	/* VARIABILI SERVER verso clientN */
	int listenfd, connfd, sockfd;
	/* 	listenfd: descrittore di ascolto
	 *	connfd: descrittore di connessione
	 */
	int len;
	struct sockaddr_in s_servaddr, s_cliaddr;
	/* 	servaddr: informazioni indirizzo server
	 *	cliaddr: informazioni indirizzo client connesso
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
	Negozio n_buf;
	Prodotto p_buf;
	char buf[BUFSIZE];
	pct_c req_c;

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
	Write(masterfd, "C", 1);	/* identificazione al server */
	Read(masterfd, buf, 1);	/* leggi esito connessione */
	if (!strncmp(buf, "0", 1))
		printf("Connesso al serverM\n");
	else {
		fputs("Connessione fallita\n", stderr);
		exit(1);
	}
	
	/*richiesta.rich = '1';
	strcpy(richiesta.query.q_neg, "Dodeca'");
	Write(sockfd, &richiesta, sizeof(richiesta));
	do {
		Read(sockfd, buf, sizeof(buf));
		if (buf[0] != '0')
			fprintf(stdout, "%s\n", buf);
	} while (buf[0] != '0');*/
	
	/*richiesta.rich = '2';
	strcpy(richiesta.query.q_neg, "Dodeca'");
	strcpy(richiesta.query.q_prop, "Amadeus");
	Write(sockfd, &richiesta, sizeof(richiesta));
	do {
		Read(sockfd, buf, sizeof(buf));
		if (buf[0] != '0' && buf[0]!='1')
			fprintf(stdout, "%s\n", buf);
	} while (buf[0] != '0' && buf[0]!='1');
	if(buf[0]=='1'){
		fputs("Nessun Risultato\n", stderr);
	
	}*/
	
	/*richiesta.rich = '3';
	strcpy(richiesta.query.q_neg, "Dodeca'");
	strcpy(richiesta.query.q_prop, "Ciro");
	strcpy(richiesta.query.q_prod, "Uova");
	Write(sockfd, &richiesta, sizeof(richiesta));
	do {
		Read(sockfd, buf, sizeof(buf));
		if (buf[0] != '0' && buf[0]!='1')
			fprintf(stdout, "%s\n", buf);
	} while (buf[0] != '0' && buf[0]!='1');
	if(buf[0]=='1')
		fputs("Nessun Risultato\n", stderr);
	else {
		if(buf[0]=='0'){
		fputs("Prodotto trovato nel negozio\n", stderr);
		}
	
	
	close(masterfd);*/
	
	/* SERVER verso client */
	/* creazione socket */
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	/* impostazioni socket */
	SetSockOpt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	/* indirizzo del server */
	s_servaddr.sin_family = AF_INET;
	s_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_servaddr.sin_port = htons(8002);

	fprintf(stdout, "IP address is: %s\n", inet_ntoa(s_servaddr.sin_addr));
	fprintf(stdout, "port is: %d\n", (int) ntohs(s_servaddr.sin_port));

	/* assegnazione indirizzo */
	Bind(listenfd, &s_servaddr);
	/* messa in ascolto */
	Listen(listenfd, 1024);

	maxd = listenfd; /* maxd è il valore massimo dei descrittori in uso */

	/* il vettore client contiene i descrittori dei socket connessi */
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1; /* inizializza il vettore di client */
	
	FD_ZERO(&active_fd_set); /* inizializza a zero l'insieme dei descrittori */
	FD_SET(listenfd, &active_fd_set); /* aggiunge il descrittore di ascolto */
	FD_SET(masterfd, &active_fd_set);
	
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
					if (Read(sockfd, &richiesta, sizeof(richiesta)) == 0) {
						switch (richiesta.rich) {
							case '1':
								Write(masterfd, &richiesta, sizeof(richiesta));
								do {
									Read(masterfd, &n_buf, sizeof(n_buf));
									Write(sockfd, &n_buf, sizeof(n_buf));
								} while (n_buf.nome_negozio[0] != '\0');
								break;
								
							case '2':
								Write(masterfd, &richiesta, sizeof(richiesta));
								do {
									Read(masterfd, &p_buf, sizeof(p_buf));
									Write(sockfd, &p_buf, sizeof(p_buf));
								} while (p_buf.nome_prodotto[0] != '\0');
								
								break;
								
							case '3':
								Write(masterfd, &richiesta, sizeof(richiesta));
									Read(masterfd, buf, 1);
									Write(sockfd, buf, 1);
								
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
	
	exit(0);
}
