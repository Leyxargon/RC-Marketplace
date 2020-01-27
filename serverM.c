#include "liste/lista.h"
#include "wrapper.h"
#include "liste/iofile.h"
#include "pct_c.h"
#include "pct_n.h"
#include <time.h>
#include <string.h>

int main(int argc, char **argv) {
	/* VARIABILI SERVER */
	int listenfd, connfd;
	/* 	listenfd: descrittore di ascolto
	 *	connfd: descrittore di connessione
	 */
	int len, pid;
	struct sockaddr_in servaddr, cliaddr;
	/* 	servaddr: informazioni indirizzo server
	 *	cliaddr: informazioni indirizzo client connesso
	 */

	/* VARIABILI LIVELLO APPLICAZIONE */
	char buf[BUFSIZE];
	pct_c req_c;
	pct_n req_n;
	login_n login;
	Utente *listaUtenti = NULL;
	FILE *dbf;
	Utente *u_tmp = NULL;
	Negozio *n_tmp = NULL;
	Prodotto *p_tmp = NULL;
	Negozio n_buf;
	Prodotto p_buf;
	
	/* LETTURA INFORMAZIONI */
	dbf = Fopen("db.txt", "r+");
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

	fprintf(stdout, "Indirizzo IP: %s\n", inet_ntoa(servaddr.sin_addr));
	fprintf(stdout, "Porta: %d\n", (int) ntohs(servaddr.sin_port));

	/* assegnazione indirizzo */
	Bind(listenfd, &servaddr);
	/* messa in ascolto */
	Listen(listenfd, 2);

	/* esecuzione server */
	while(1) {
		/* accettazione nuova richiesta */
		len = sizeof(cliaddr);
		connfd = Accept(listenfd, &cliaddr, &len);

		/* fork per la gestione della connessione */
		if (!(pid = Fork())) {
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
						case '1':	/* Crea un nuovo negozio virtuale */
							u_tmp = ricercaUtente(listaUtenti, req_n.user);		/* seleziona l'utente */
							if ((n_tmp = ricercaNegozio(u_tmp -> negozi, req_n.query.q_neg)) == NULL)  /* assicura che il negozio non esista */
								u_tmp -> negozi = inserisciNegozio(u_tmp -> negozi, req_n.query.q_neg);
							else
								Write(connfd, "1", 1);	/* errore: negozio già esistente */
							
							if ((n_tmp = ricercaNegozio(u_tmp -> negozi, req_n.query.q_neg)) != NULL) {
								visualizzaDati(listaUtenti);
								Write(connfd, "0", 1);	/* operazione effettuata correttamente */
							}
							else
								Write(connfd, "1", 1);	/* errore */
							break;
						case '2':	/* Elimina un negozio virtuale */
							u_tmp = ricercaUtente(listaUtenti, req_n.user);		/* seleziona l'utente */
							if ((n_tmp = ricercaNegozio(u_tmp -> negozi, req_n.query.q_neg)) != NULL)  /* assicura che il negozio non esista */
								u_tmp -> negozi = eliminaNegozio(u_tmp -> negozi, req_n.query.q_neg);
							else
								Write(connfd, "1", 1); /* errore: negozio inesistente */
							if ((n_tmp = ricercaNegozio(u_tmp -> negozi, req_n.query.q_neg)) == NULL)
								Write(connfd, "0", 1);	/* operazione effettuata correttamente */
							else
								Write(connfd, "1", 1);	/* errore */
							visualizzaDati(listaUtenti);
							break;
						case '3':	/* Aggiungi un prodotto in un negozio virtuale */
							u_tmp = ricercaUtente(listaUtenti, req_n.user);		/* seleziona l'utente */
							if ((n_tmp = ricercaNegozio(u_tmp -> negozi, req_n.query.q_neg)) != NULL) {
								if ((p_tmp = ricercaProdotto(n_tmp -> prodotti, req_n.query.q_prod)) == NULL) {
									n_tmp -> prodotti = inserisciProdotto(n_tmp -> prodotti, req_n.query.q_prod);
									Write(connfd, "0", 1);
								}
								else
									Write(connfd, "1", 1);	/* prodotto già esistente in quel negozio */
							}
							else
								Write(connfd, "1", 1);	/* negozio inesistente */
							visualizzaDati(listaUtenti);
							break;
						case '4':	/* Rimuovi un prodotto in un negozio virtuale */
							u_tmp = ricercaUtente(listaUtenti, req_n.user);		/* seleziona l'utente */
							if ((n_tmp = ricercaNegozio(u_tmp -> negozi, req_n.query.q_neg)) != NULL) {
								if ((p_tmp = ricercaProdotto(n_tmp -> prodotti, req_n.query.q_prod)) != NULL) {
									n_tmp -> prodotti = eliminaProdotto(n_tmp -> prodotti, req_n.query.q_prod);
									Write(connfd, "0", 1);	/* operazione effettuata correttamente */
								}
								else
									Write(connfd, "1", 1); /* prodotto inesistente */
							}
							else
								Write(connfd, "1", 1); /* negozio inesistente */
							visualizzaDati(listaUtenti);
							break;
						case '5':	/* Visualizza i negozi virtuali di un negoziante */
							u_tmp = ricercaUtente(listaUtenti, req_n.user);
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
							u_tmp = ricercaUtente(listaUtenti, req_n.user);
							if ((n_tmp = ricercaNegozio(u_tmp -> negozi, req_n.query.q_neg)) != NULL) {
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
						case '9':	/* Richiesta di login */
							Write(connfd, "0", 1);
							Read(connfd, &login, sizeof(login));
							if ((u_tmp = ricercaUtente(listaUtenti, login.user)) == NULL) {
								fputs("Utente non valido.\n", stderr);
								Write(connfd, "1", 1); /* utente non valido */
							}
							else {
								if (!strcmp(u_tmp -> password, login.pass)) {
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
										n_buf = *n_tmp;
										Write(connfd, &n_buf, sizeof(n_buf));
										n_tmp = n_tmp -> next;
									}
									u_tmp = u_tmp -> next;
								}
								n_buf.nome_negozio[0] = '\0';
								Write(connfd, &n_buf, sizeof(n_buf));
								break;
								
							case '2':
								fputs("Ricevuta richiesta 2 da serverC\n", stdout);
								u_tmp = listaUtenti;
								n_tmp = u_tmp -> negozi;
								if ((n_tmp = ricercaNegozio(u_tmp -> negozi, req_c.query.q_neg)) != NULL) {
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
								n_tmp = u_tmp -> negozi; 
								if ((n_tmp = ricercaNegozio(u_tmp -> negozi, req_c.query.q_neg)) != NULL)
									p_tmp = n_tmp -> prodotti;
								else
									Write(connfd, "1", 1);
								if ((p_tmp = ricercaProdotto(p_tmp, req_c.query.q_prod)) != NULL)
									Write(connfd, "0", 1); 
								else
									Write(connfd, "1", 1);
								break;
						}
					}
				}
				else {
					/* server non riconosciuto */
					Write(connfd, "0", 1);
					fputs("Richiesta non riconosciuta. Connessione chiusa.\n", stderr);
					close(connfd);
				}
			}
			exit(0);
		}
		else
			close(connfd);
	}
	exit(0);
}
