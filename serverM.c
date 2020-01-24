#include "liste/lista.h"
#include "wrapper.h"
#include "liste/iofile.h"
#include "pct_c.c"
#include "pct_n.c"
#include <time.h>
#include <string.h>

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

			Read(connfd, buf, 1); /* identificazione server */
			if (!strncmp(buf, "N", 1)) {
				/* connessione con serverN */
				Write(connfd, "0", 1);
				fputs("Connessione con serverN.\n", stderr);
				while (1) {
					if (Read(connfd, &req_n, sizeof(req_n)) != 0) {
						fputs("Connessione persa con serverN.\n", stderr);
						exit(1);
					}
					switch (req_n.rich) {
						case '1':
							/* crea un nuovo negozio virtuale */
							u_tmp = ricercaUtente(listaUtenti, req_n.user);		/* seleziona l'utente */
							if ((n_tmp = ricercaNegozio(u_tmp -> negozi, req_n.query.q_neg)) == NULL)  /* assicura che il negozio non esista */
								u_tmp -> negozi = inserisciNegozio(u_tmp -> negozi, req_n.query.q_neg);
							else
								Write(connfd, "1", 1);	/* errore: negozio già esistente */
							
							if ((n_tmp = ricercaNegozio(u_tmp -> negozi, req_n.query.q_neg)) != NULL) {
								fputs("Inserito nuovo negozio, ecco la lista attuale:\n", stdout);
								visualizzaDati(listaUtenti);
								Write(connfd, "0", 1);	/* operazione effettuata correttamente */
							}
							else
								Write(connfd, "1", 1);	/* errore */
							break;
						case '2':
							/* Elimina un negozio virtuale */
							u_tmp = ricercaUtente(listaUtenti, req_n.user);		/* seleziona l'utente */
							if ((n_tmp = ricercaNegozio(u_tmp -> negozi, req_n.query.q_neg)) != NULL)  /* assicura che il negozio non esista */
								u_tmp -> negozi = eliminaNegozio(u_tmp -> negozi, req_n.query.q_neg);
							else
								Write(connfd, "1", 1); /* errore: negozio inesistente */
							if ((n_tmp = ricercaNegozio(u_tmp -> negozi, req_n.query.q_neg)) == NULL)
								Write(connfd, "0", 1);	/* operazione effettuata correttamente */
							else
								Write(connfd, "1", 1);	/* errore */
							break;
						case '3':
							/* Aggiungi un prodotto in un negozio virtuale */
							u_tmp = ricercaUtente(listaUtenti, req_n.user);		/* seleziona l'utente */
							if ((n_tmp = ricercaNegozio(u_tmp -> negozi, req_n.query.q_neg)) != NULL) {
								if ((p_tmp = ricercaProdotto(n_tmp -> prodotti, req_n.query.q_prod)) == NULL)
									n_tmp -> prodotti = inserisciProdotto(n_tmp -> prodotti, req_n.query.q_prod);
								else
									Write(connfd, "1", 1);	/* prodotto già esistente in quel negozio */
							}
							else
								Write(connfd, "1", 1);	/* negozio inesistente */
							if ((p_tmp = ricercaProdotto(n_tmp -> prodotti, req_n.query.q_prod)) != NULL)
								Write(connfd, "0", 1);	/* operazione effettuata correttamente */
							else
								Write(connfd, "1", 1);	/* errore */
							visualizzaDati(listaUtenti);
							break;
						case '4':
							/* Rimuovi un prodotto in un negozio virtuale */
							u_tmp = ricercaUtente(listaUtenti, req_n.user);		/* seleziona l'utente */
							if ((n_tmp = ricercaNegozio(u_tmp -> negozi, req_n.query.q_neg)) != NULL) {
								if ((p_tmp = ricercaProdotto(n_tmp -> prodotti, req_n.query.q_prod)) != NULL)
									n_tmp -> prodotti = eliminaProdotto(n_tmp -> prodotti, req_n.query.q_prod);
								else
									Write(connfd, "1", 1); /* prodotto inesistente */
							}
							else
								Write(connfd, "1", 1); /* negozio inesistente */
							if ((p_tmp = ricercaProdotto(n_tmp -> prodotti, req_n.query.q_prod)) == NULL)
								Write(connfd, "0", 1);	/* operazione effettuata correttamente */
							else
								Write(connfd, "1", 1);	/* errore */
							visualizzaDati(listaUtenti);
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
			}
			else {
				if (!strncmp(buf, "C", 1)) {
					/* connessione con serverC */
					Write(connfd, "0", 1);
					fputs("Connessione con serverC.\n", stderr);
					while (1) {
						if (Read(connfd, &req_c, sizeof(req_c)) != 0) {
							fputs("Connessione persa con serverC.\n", stderr);
							exit(1);
						}
						switch(req_c.rich) {
							case '1':
								fputs("Ricevuta richiesta 1 da serverC\n", stdout);
								u_tmp = listaUtenti;
								while (u_tmp != NULL) {
									n_tmp = u_tmp -> negozi;
									while (n_tmp != NULL) {
										sprintf(buf, "%s di %s", n_tmp -> nome_negozio, u_tmp -> username);
										Write(connfd, buf, sizeof(buf));
										n_tmp = n_tmp -> next;
									}
									u_tmp = u_tmp -> next;
								}
								Write(connfd, "0", sizeof(buf));
								break;
								
							case '2':
								fputs("Ricevuta richiesta 2 da serverC\n", stdout);
								if((u_tmp = ricercaUtente(listaUtenti, req_c.query.q_prop))!=NULL){
									n_tmp = u_tmp -> negozi;
									if ((n_tmp=ricercaNegozio(u_tmp->negozi, req_c.query.q_neg))!=NULL){
										p_tmp=n_tmp->prodotti;
										while (p_tmp != NULL) {
											sprintf(buf, "%s ", p_tmp -> nome_prodotto);
											Write(connfd, buf, sizeof(buf));
											p_tmp = p_tmp -> next;
										}
										Write(connfd, "0", sizeof(buf));
									}
									else
										//negozio non trovato//
										Write(connfd, "1", sizeof(buf));
								}
								else
									//utente non trovato//
									Write(connfd, "1", sizeof(buf));

								break;
								
							case '3':
								fputs("Ricevuta richiesta 3 da serverC\n" , stdout);
								if((u_tmp = ricercaUtente(listaUtenti, req_c.query.q_prop))!=NULL){ //if non esiste chiudi con cod.1
									n_tmp = u_tmp -> negozi; 
									if((n_tmp=ricercaNegozio(u_tmp->negozi, req_c.query.q_neg))!=NULL) //if non esiste chiudi con cod.1
										p_tmp=n_tmp->prodotti;
									else
										Write(connfd, "1", sizeof(buf));
									if((p_tmp=ricercaProdotto(p_tmp, req_c.query.q_prod))!=NULL)//if esiste chiudi con cod 0 else chiudi con cod.1
										Write(connfd, "0", sizeof(buf)); 
									else
										Write(connfd, "1", sizeof(buf));
								}
								else 
									Write(connfd, "1", sizeof(buf));
								break;
						}
					}
				}
				else {
					/* richiesta sconosciuta */
					Write(connfd, "0", 1);
					fputs("Richiesta non riconosciuta. Connessione chiusa.\n", stderr);
					close(connfd);
				}
			}
			exit(0);
		}
		else /* parent */
			close(connfd);
	}
	exit(0);
}
