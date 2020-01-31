#include "serverM.h"

int main(int argc, char **argv) {
	/* VARIABILI SERVER */
	int listenfd, connfd;
	/* 	listenfd: descrittore di ascolto
	 *	connfd: descrittore di connessione
	 */
	struct sockaddr_in servaddr, cliaddr;
	/* 	servaddr: informazioni indirizzo server
	 *	cliaddr: informazioni indirizzo client connesso
	 */
	int len = sizeof(cliaddr);;

	/* VARIABILI LIVELLO APPLICAZIONE */
	Utente *listaUtenti = NULL;
	FILE *dbf;
	char buf[BUFSIZE];
	pthread_t C, N;
	pthread_args args;

	/* LETTURA INFORMAZIONI */
	dbf = Fopen("db.txt", "r");
	fputs("Caricamento dati in corso...\n", stdout);
	if ((listaUtenti = importaDati(dbf)) == NULL) {
		fputs("Errore nel caricamento dei dati.\n", stderr);
		exit(1);
	}
	else
		fputs("Dati caricati correttamente.\n", stdout);
	fclose(dbf);
    visualizzaDati(listaUtenti);
	/************/

	/* CONNESSIONE */
	/* creazione socket */
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	/* indirizzo del server */
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(8000);

	fprintf(stdout, "Indirizzo IP: %s\n", inet_ntoa(servaddr.sin_addr));
	fprintf(stdout, "Porta: %d\n", (int) ntohs(servaddr.sin_port));

	/* assegnazione indirizzo */
	Bind(listenfd, &servaddr);
	/* messa in ascolto */
	Listen(listenfd, 2);

	/* esecuzione server */
	while (1) {
		/* accettazione nuova richiesta */
		connfd = Accept(listenfd, &cliaddr, &len);

		Read(connfd, buf, 1); /* identificazione server */
		if (buf[0] == 'N') {
			args.connfd = connfd;
			args.dbf = dbf;
			args.listaUtenti = listaUtenti;
			Write(connfd, "0", 1);
			fputs("Connessione con serverN.\n", stdout);
			if (pthread_create(&N, NULL, serverN, (void *) &args) < 0) {
				perror("pthread_create");
				exit(1);
			}
		}
		else if (buf[0] == 'C') {
			args.connfd = connfd;
			args.listaUtenti = listaUtenti;
			Write(connfd, "0", 1);
			fputs("Connessione con serverC.\n", stdout);
			if (pthread_create(&C, NULL, serverC, (void *) &args) < 0) {
				perror("pthread_create");
				exit(1);
			}
		}
		else {
			Write(connfd, "0", 1);
			fputs("Server non riconosciuto.\n", stderr);
			close(connfd);
		}
	}

	exit(0);
}