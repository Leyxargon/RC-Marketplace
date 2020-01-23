/**
 * Semplice programma che invia ai client il Daytime del server.
 *
 * Il programma è abilitato alla gestione di più richieste in parallelo,
 * mediante la tecnica della programmazione concorrente. Il processo genitore
 * si occupa della gestione delle richieste (socket di richieste), mentre i
 * processi figli si occupano del completamento delle richieste (socket accettati).
 * @author Dario Musella
 */

#include "wrapper.c"
#include <time.h>
#include <string.h>

int main(int argc, char **argv) {
	/* gestione errore */
	if (argc != 2) {
		fprintf(stderr,"usage: %s <IPaddress>\n",argv[0]);
		exit(1);
	}
	
	
	
	/* VARIABILI SERVER verso clientN */
	int listenfd, connfd;					
	/* 	listenfd: descrittore di ascolto
	 *	connfd: descrittore di connessione
	 */
	int len, pid, verbose = 0;
	struct sockaddr_in servaddr, cliaddr;
	/* 	servaddr: informazioni indirizzo server
	 *	cliaddr: informazioni indirizzo client connesso
	 */

	/* VARIABILI CLIENT verso serverM */
	struct sockaddr_in servaddr;
	struct hostent *cliaddr;
	int sockfd;
	
	char **alias;
	char *addr;
	 
	/* VARIABILI LIVELLO APPLICAZIONE */
	Utente *u_tmp = NULL;
	Negozio *n_tmp = NULL;
	Prodotto *p_tmp = NULL;
	char buf[BUFSIZE];
	char buff[BUFSIZE];
	pct_c req_c;
	pct_n req_n;
	login_n login;
	

	/* CLIENT verso serverM */
	/* creazione socket */
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8000);	
	
	/* lettura indirizzo IP */
	cliaddr = GetHostByName(argv[1]);
    alias = cliaddr->h_addr_list;	/* legge gli indirizzi IP dall'eventuale indirizzo simbolico */
	addr = (char *) inet_ntop(cliaddr->h_addrtype, *alias, buff, sizeof(buff));
	Inet_pton(AF_INET, addr, &servaddr.sin_addr);
	
	/* connessione al server */
	Connect(sockfd, &servaddr);

	Write(connfd, "N", 1);	/* identificazione al server */
	Read(connfd, buf, sizeof(buf));	/* leggi esito connessione */
	if (!strcmp(buf, '0'))
		printf("Connessione con successo\n");
	else
		printf("Connessione fallita\n");
	
	
	/* SERVER verso clientN */
	/* creazione socket */
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	/* indirizzo del server */
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(8000);
	
	fprintf(stdout, "IP address is: %s\n", inet_ntoa(servaddr.sin_addr));
	fprintf(stdout, "port is: %d\n", (int) ntohs(servaddr.sin_port));
	
	/* assegnazione indirizzo */
	Bind(listenfd, &servaddr);
	/* messa in ascolto */
	Listen(listenfd, 1024);
	
	/* esecuzione server */
	while(1) {
		/* accettazione nuova richiesta */
		len = sizeof(cliaddr);
		connfd = Accept(listenfd, &cliaddr, &len);
		
		/* fork per la gestione della connessione */
		if (!(pid = Fork())) { /* child */
			close(listenfd);


			exit(0);
		}
		else /* parent */
			close(connfd);
	}
	exit(0);
}
