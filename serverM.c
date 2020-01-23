/**
 * Semplice programma che invia ai client il Daytime del server.
 *
 * Il programma è abilitato alla gestione di più richieste in parallelo,
 * mediante la tecnica della programmazione concorrente. Il processo genitore
 * si occupa della gestione delle richieste (socket di richieste), mentre i
 * processi figli si occupano del completamento delle richieste (socket accettati).
 * @author Dario Musella
 */
#include "liste/lista.h"
#include "wrapper.h"
#include "liste/iofile.h"
#include "pct_c.c"
#include "pct_n.c"
#include <time.h>
#include <string.h>
#define BUFSIZE 2048

int main(int argc, char **argv) {
	/* VARIABILI SERVER */
	int listenfd, connfd;
	/* 	listenfd: descrittore di ascolto
	 *	connfd: descrittore di connessione
	 */
	int len, pid, verbose = 0;
	struct sockaddr_in servaddr, cliaddr;
	/* 	servaddr: informazioni indirizzo server
	 *	cliaddr: informazioni indirizzo client connesso
	 */

	/* VARIABILI LIVELLO APPLICAZIONE */
	char buff[BUFSIZE];
	char buf[BUFSIZE];
	pct_c req_c;
	pct_n req_n;
	login_n login;
	Utente *listaUtenti = NULL;
	FILE *dbf;
	Utente *u_tmp = NULL;
	Negozio *n_tmp = NULL;
	Prodotto *p_tmp = NULL;

	/* gestione errore */
	if (argc > 1) {
		if (strcmp(argv[1], "-v") && strcmp(argv[1], "--verbose")) {
			fprintf(stderr,"Usage: %s [flag]\n\n  -v, --verbose\t\tverbose mode showing 'IP:port' client connections\n",argv[0]);
			exit(1);
		}
		else
			verbose++; /* flag per la modalità verbosa */
	}
	/* LETTURA INFORMAZIONI */

	if ((dbf = fopen("db.txt", "r+")) == NULL) {
		perror("fopen");
		exit(1);
	}
	fputs("Caricamento dati in corso...\n", stdout);
	if ((listaUtenti = importaDati(dbf)) == NULL) {
		fputs("Errore nel caricamento dei dati.\n", stderr);
		exit(1);
	}
	else
		fputs("Dati caricati correttamente.\n", stdout);
    visualizzaDati(listaUtenti);
	/************/

	/* CONNESSIONE */
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

			Read(connfd, buf, sizeof(buf)); /* identificazione server */
			if (strncmp(buf, "N", 1)) {
				/* connessione con serverN */
				fputs("Connessione con serverN.\n", stderr);
				Write(connfd, "0", 1);
				Read(connfd, &req_n, sizeof(req_n));
				switch (req_n.rich) {
					case '1':
						/* crea un nuovo negozio virtuale */
						u_tmp = ricercaUtente(listaUtenti, req_n.user);		/* seleziona l'utente */
						u_tmp -> negozi = inserisciNegozio(u_tmp -> negozi, req_n.query.q_neg);
						if ((n_tmp = ricercaNegozio(u_tmp -> negozi, req_n.query.q_neg)) != NULL)
							Write(connfd, "0", 1);	/* operazione effettuata correttamente */
						else
							Write(connfd, "1", 1);	/* errore */
						break;
					case '2':
						/* Elimina un negozio virtuale */
						u_tmp = ricercaUtente(listaUtenti, req_n.user);		/* seleziona l'utente */
						u_tmp -> negozi = eliminaNegozio(u_tmp -> negozi, req_n.query.q_neg);
						if ((n_tmp = ricercaNegozio(u_tmp -> negozi, req_n.query.q_neg)) == NULL)
							Write(connfd, "0", 1);	/* operazione effettuata correttamente */
						else
							Write(connfd, "1", 1);	/* errore */
						break;
					case '3':
						/* Aggiungi un prodotto in un negozio virtuale */
						u_tmp = ricercaUtente(listaUtenti, req_n.user);		/* seleziona l'utente */
						n_tmp = ricercaNegozio(u_tmp -> negozi, req_n.query.q_neg);
						n_tmp -> prodotti = inserisciProdotto(n_tmp -> prodotti, req_n.query.q_prod);
						if ((p_tmp = ricercaProdotto(n_tmp -> prodotti, req_n.query.q_prod)) != NULL)
							Write(connfd, "0", 1);	/* operazione effettuata correttamente */
						else
							Write(connfd, "1", 1);	/* errore */
						break;
					case '4':
						/* Rimuovi un prodotto in un negozio virtuale */
						u_tmp = ricercaUtente(listaUtenti, req_n.user);		/* seleziona l'utente */
						n_tmp = ricercaNegozio(u_tmp -> negozi, req_n.query.q_neg);
						n_tmp -> prodotti = eliminaProdotto(n_tmp -> prodotti, req_n.query.q_prod);
						if ((p_tmp = ricercaProdotto(n_tmp -> prodotti, req_n.query.q_prod)) == NULL)
							Write(connfd, "0", 1);	/* operazione effettuata correttamente */
						else
							Write(connfd, "1", 1);	/* errore */
						break;
					case '5':
						Write(connfd, "0", 1);	/* ok, letta richiesta di login, leggere le credenziali */
						Read(connfd, &login, sizeof(login));
						if ((u_tmp = ricercaUtente(listaUtenti, login.user)) == NULL)
							Write(connfd, "1", 1); /* utente non valido */
						else {
							if (!strcmp(u_tmp -> password, login.pass))
								Write(connfd, "0", 1);
							else
								Write(connfd, "2", 1); /* password non valida */
						}
						break;
					default:
						fputs("Richiesta non valida.\n", stderr);
						break;
				}
			}
			else {
				if (strncmp(buf, "C", 1)) {
					/* connessione con serverC */
					fputs("Connessione con serverC.\n", stderr);

				}
				else {
					/* richiesta sconosciuta */
					fputs("Richiesta non riconosciuta. Connessione chiusa.\n", stderr);
					close(connfd);
				}
			}
			Write(connfd, buff, sizeof(buff));

			if (verbose) {
				/* effettua il log dei client che richiedono */
				inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff));
				fprintf(stdout, "Request from host %s, port %d\n", buff, ntohs(cliaddr.sin_port));
			}
			exit(0);
		}
		else /* parent */
			close(connfd);
	}
	exit(0);
}
