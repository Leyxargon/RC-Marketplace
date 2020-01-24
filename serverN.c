#include "liste/lista.h"
#include "wrapper.h"
#include "liste/iofile.h"
#include "pct_c.c"
#include "pct_n.c"
#include <time.h>
#include <string.h>

int main(int argc, char **argv) {
    int verbose = 0;
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
	int len, pid;
	struct sockaddr_in s_servaddr, s_cliaddr;
	/* 	servaddr: informazioni indirizzo server
	 *	cliaddr: informazioni indirizzo client connesso
	 */

	/* VARIABILI CLIENT verso serverM */
	struct sockaddr_in c_servaddr;
	struct hostent *c_cliaddr;
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
	c_servaddr.sin_family = AF_INET;
	c_servaddr.sin_port = htons(8000);

	/* lettura indirizzo IP */
	c_cliaddr = GetHostByName(argv[1]);
    alias = c_cliaddr->h_addr_list;	/* legge gli indirizzi IP dall'eventuale indirizzo simbolico */
	addr = (char *) inet_ntop(c_cliaddr->h_addrtype, *alias, buff, sizeof(buff));
	Inet_pton(AF_INET, addr, &c_servaddr.sin_addr);

	/* connessione al server */
	Connect(sockfd, &c_servaddr);

	fputs("Connessione al serverM\n", stdout);
	Write(sockfd, "N", 1);	/* identificazione al server */
	Read(sockfd, buf, 1);	/* leggi esito connessione */
	if (!strncmp(buf, "0", 1))
		printf("Connesso al serverM\n");
	else {
		fputs("Connessione fallita\n", stderr);
		exit(1);
	}
	
	pct_n richiesta;
	richiesta.rich = '4';
	strcpy(richiesta.user, "Ciro");
	strcpy(richiesta.query.q_neg, "Dodeca'");
	strcpy(richiesta.query.q_prod, "Frittata");
	Write(sockfd, &richiesta, sizeof(richiesta));
	Read(sockfd, buf, 1);
	if (buf[0] == '0') {
		fputs("Operazione effettuata con successo.\n", stdout);
	}
	else
		fputs("Operazione fallita.\n", stdout);
	
	richiesta.rich = '1';
	strcpy(richiesta.user, "Ciro");
	strcpy(richiesta.query.q_neg, "Patatineria");
	Write(sockfd, &richiesta, sizeof(richiesta));
	Read(sockfd, buf, 1);
	if (buf[0] == '0') {
		fputs("Operazione effettuata con successo.\n", stdout);
	}
	else
		fputs("Operazione fallita.\n", stdout);
	
	richiesta.rich = '3';
	strcpy(richiesta.user, "Ciro");
	strcpy(richiesta.query.q_neg, "Patatineria");
	strcpy(richiesta.query.q_prod, "Patatine buone buone");
	Write(sockfd, &richiesta, sizeof(richiesta));
	Read(sockfd, buf, 1);
	if (buf[0] == '0') {
		fputs("Operazione effettuata con successo.\n", stdout);
	}
	else
		fputs("Operazione fallita.\n", stdout);
	
	// close(sockfd);

	/* SERVER verso clientN */
	/* creazione socket */
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	/* indirizzo del server */
	s_servaddr.sin_family = AF_INET;
	s_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_servaddr.sin_port = htons(8001);

	fprintf(stdout, "IP address is: %s\n", inet_ntoa(s_servaddr.sin_addr));
	fprintf(stdout, "port is: %d\n", (int) ntohs(s_servaddr.sin_port));

	/* assegnazione indirizzo */
	Bind(listenfd, &s_servaddr);
	/* messa in ascolto */
	Listen(listenfd, 1024);

	/* esecuzione server */
	while(1) {
		/* accettazione nuova richiesta */
		len = sizeof(s_cliaddr);
		connfd = Accept(listenfd, &s_cliaddr, &len);

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
