#include "serverM.h"

void *serverC(void *connfd);
void *serverN(void *connfd);

typedef struct PTHREAD_ARGS {
    int connfd;
	FILE *dbf;
	Utente *listaUtenti;
}pthread_args;

int main(int argc, char **argv) {
	/* VARIABILI SERVER */
	int listenfd, connfd;
	/* 	listenfd: descrittore di ascolto
	 *	connfd: descrittore di connessione
	 */
	int len;
	struct sockaddr_in servaddr, cliaddr;
	/* 	servaddr: informazioni indirizzo server
	 *	cliaddr: informazioni indirizzo client connesso
	 */

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
		len = sizeof(cliaddr);
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

void *serverC(void *args) {
	Utente *listaUtenti = ((pthread_args *) args) -> listaUtenti;
	int connfd = ((pthread_args *) args) -> connfd;
	Utente *u_tmp = NULL;
	Negozio *n_tmp = NULL;
	Prodotto *p_tmp = NULL;
	Negozio n_buf;
	Prodotto p_buf;
	pct_c richiesta;

	while (1) {
		if (!Read(connfd, &richiesta, sizeof(richiesta)) != 0) {
			switch(richiesta.rich) {
				case '1':	/* Visualizza l'elenco dei negozi virtuali */
					fputs("Ricevuta richiesta 1 da serverC\n", stdout);
					u_tmp = listaUtenti;
					while (u_tmp != NULL) {
						n_tmp = u_tmp -> negozi;
						while (n_tmp != NULL) {
							n_buf = *n_tmp;
							Write(connfd, &n_buf, sizeof(n_buf));
							n_tmp = n_tmp -> next;
						}
						u_tmp = u_tmp -> next;
					}
					n_buf.nome_negozio[0] = '\0';
					Write(connfd, &n_buf, sizeof(n_buf));

					break;
				case '2':	/* Visualizza i prodotti di un negozio */
					fputs("Ricevuta richiesta 2 da serverC\n", stdout);
					u_tmp = listaUtenti;
					n_tmp = NULL;
					while (u_tmp != NULL && n_tmp == NULL) { /* ricerca negozio */
						n_tmp = ricercaNegozio(u_tmp -> negozi, richiesta.query.q_neg);
						u_tmp = u_tmp -> next;
					}
					if (n_tmp != NULL) {
						p_tmp = n_tmp -> prodotti;
						while (p_tmp != NULL) {
							p_buf = *p_tmp;
							Write(connfd, &p_buf, sizeof(p_buf));
							p_tmp = p_tmp -> next;
							}
					}
					p_buf.nome_prodotto[0] = '\0';
					Write(connfd, &p_buf, sizeof(p_buf));

					break;
				case '3':
					fputs("Ricevuta richiesta 3 da serverC\n" , stdout);
					u_tmp = listaUtenti;
					n_tmp = NULL;
					while (u_tmp != NULL && n_tmp == NULL) { /* ricerca negozio */
						n_tmp = ricercaNegozio(u_tmp -> negozi, richiesta.query.q_neg);
						u_tmp = u_tmp -> next;
					}
					if (n_tmp != NULL) {
						if ((p_tmp = ricercaProdotto(n_tmp -> prodotti, richiesta.query.q_prod)) != NULL)
							Write(connfd, "0", 1);
						else
							Write(connfd, "1", 1);
					}
					else
						Write(connfd, "1", 1);

					break;
				default:
					fputs("Richiesta non valida.\n", stderr);
			}
		}
		else {
			fputs("Connessione persa con serverC.\n", stderr);
			pthread_exit(NULL);
		}
	}
	pthread_exit(NULL);
}

void *serverN(void *args) {
	Utente *listaUtenti = ((pthread_args *) args) -> listaUtenti;
	FILE *dbf = ((pthread_args *) args) -> dbf;
	int connfd = ((pthread_args *) args) -> connfd;
	char buf[BUFSIZE];
	Utente *u_tmp = NULL;
	Negozio *n_tmp = NULL;
	Prodotto *p_tmp = NULL;
	pct_n richiesta;
	pct_l credenz;

	while (1) {
		if (!Read(connfd, &richiesta, sizeof(richiesta))) {
			switch (richiesta.rich) {
				case '1':	/* Crea un nuovo negozio virtuale */
					u_tmp = ricercaUtente(listaUtenti, richiesta.user);		/* seleziona l'utente */
					if ((n_tmp = ricercaNegozio(u_tmp -> negozi, richiesta.query.q_neg)) == NULL) { /* assicura che il negozio non esista */
						u_tmp -> negozi = inserisciNegozio(u_tmp -> negozi, richiesta.query.q_neg);
						commit(dbf, listaUtenti);
						visualizzaDati(listaUtenti);
						Write(connfd, "0", 1);	/* operazione effettuata correttamente */
					}
					else
						Write(connfd, "1", 1);	/* negozio già esistente */

					break;
				case '2':	/* Elimina un negozio virtuale */
					u_tmp = ricercaUtente(listaUtenti, richiesta.user);		/* seleziona l'utente */
					if ((n_tmp = ricercaNegozio(u_tmp -> negozi, richiesta.query.q_neg)) != NULL) {  /* assicura che il negozio non esista */
						u_tmp -> negozi = eliminaNegozio(u_tmp -> negozi, richiesta.query.q_neg);
						commit(dbf, listaUtenti);
						visualizzaDati(listaUtenti);
						Write(connfd, "0", 1);	/* operazione effettuata correttamente */
					}
					else
						Write(connfd, "1", 1); /* negozio inesistente */

					break;
				case '3':	/* Aggiungi un prodotto in un negozio virtuale */
					u_tmp = ricercaUtente(listaUtenti, richiesta.user);		/* seleziona l'utente */
					if ((n_tmp = ricercaNegozio(u_tmp -> negozi, richiesta.query.q_neg)) != NULL) {
						if ((p_tmp = ricercaProdotto(n_tmp -> prodotti, richiesta.query.q_prod)) == NULL) {
							n_tmp -> prodotti = inserisciProdotto(n_tmp -> prodotti, richiesta.query.q_prod);
							commit(dbf, listaUtenti);
							visualizzaDati(listaUtenti);
							Write(connfd, "0", 1);	/* operazione effettuata correttamente */
						}
						else
							Write(connfd, "1", 1);	/* prodotto già esistente in quel negozio */
					}
					else
						Write(connfd, "1", 1);	/* negozio inesistente */

					break;
				case '4':	/* Rimuovi un prodotto in un negozio virtuale */
					u_tmp = ricercaUtente(listaUtenti, richiesta.user);		/* seleziona l'utente */
					if ((n_tmp = ricercaNegozio(u_tmp -> negozi, richiesta.query.q_neg)) != NULL) {
						if ((p_tmp = ricercaProdotto(n_tmp -> prodotti, richiesta.query.q_prod)) != NULL) {
							n_tmp -> prodotti = eliminaProdotto(n_tmp -> prodotti, richiesta.query.q_prod);
							commit(dbf, listaUtenti);
							visualizzaDati(listaUtenti);
							Write(connfd, "0", 1);	/* operazione effettuata correttamente */
						}
						else
							Write(connfd, "1", 1); /* prodotto inesistente */
					}
					else
						Write(connfd, "1", 1); /* negozio inesistente */

					break;
				case '5':	/* Visualizza i negozi virtuali di un negoziante */
					u_tmp = ricercaUtente(listaUtenti, richiesta.user);
					n_tmp = u_tmp -> negozi;
					while (n_tmp != NULL) {
						sprintf(buf, "%s", n_tmp -> nome_negozio);
						Write(connfd, buf, sizeof(buf));
						n_tmp = n_tmp -> next;
					}
					buf[0] = '\0';
					Write(connfd, buf, sizeof(buf));

					break;
				case '6':	/* Visualizza i prodotti di un negozio virtuale di un negoziante */
					u_tmp = ricercaUtente(listaUtenti, richiesta.user);
					if ((n_tmp = ricercaNegozio(u_tmp -> negozi, richiesta.query.q_neg)) != NULL) {
						p_tmp = n_tmp -> prodotti;
						while (p_tmp != NULL) {
							sprintf(buf, "%s", p_tmp -> nome_prodotto);
							Write(connfd, buf, sizeof(buf));
							p_tmp = p_tmp -> next;
						}
					}
					buf[0] = '\0';
					Write(connfd, buf, sizeof(buf));

					break;
				case '8':	/* Richiesta di registrazione */
					Write(connfd, "0", 1);
					Read(connfd, &credenz, sizeof(credenz));
					if (ricercaUtente(listaUtenti, credenz.user) == NULL) {
						listaUtenti = inserisciUtente(listaUtenti, credenz.user, credenz.pass);
						commit(dbf, listaUtenti);
						visualizzaDati(listaUtenti);
						Write(connfd, "0", 1);	/* operazione effettuata correttamente */
					}
					else
						Write(connfd, "1", 1); /* utente già esistente */

					break;
				case '9':	/* Richiesta di login */
					Write(connfd, "0", 1);
					Read(connfd, &credenz, sizeof(credenz));
					if ((u_tmp = ricercaUtente(listaUtenti, credenz.user)) == NULL) {
						fputs("Utente non valido.\n", stderr);
						Write(connfd, "1", 1); /* utente non valido */
					}
					else {
						if (!strcmp(u_tmp -> password, credenz.pass)) {
							fputs("Autenticazione confermata.\n", stdout);
							Write(connfd, "0", 1);
						}
						else {
							fputs("Password non valida.\n", stderr);
							Write(connfd, "2", 1); /* password non valida */
						}
					}

					break;
				default:
					fputs("Richiesta non valida.\n", stderr);

				break;
			}
		}
		else {
			fputs("Connessione persa con serverN.\n", stderr);
			pthread_exit(NULL);
		}
	}
	pthread_exit(NULL);
}
