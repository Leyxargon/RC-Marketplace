#include "liste/lista.h"
#include "wrapper.h"
#include "liste/iofile.h"
#include "pct_r.h"
#include "pct_l.h"

int main(int argc, char **argv) {
	/* VARIABILI SERVER */
	int listenfd, connfd, sockfd;
	/* 	listenfd: 	descrittore di ascolto
	 *	connfd: 	descrittore di connessione
	 *	sockfd: 	descrittore di servizio
	 */
	int len;
	struct sockaddr_in servaddr, cliaddr;
	/* 	servaddr: informazioni indirizzo server
	 *	cliaddr: informazioni indirizzo client connesso
	 */
	int option = 1, i, maxi = -1, maxd, ready, server[2];
	fd_set active_fd_set, read_fd_set;	/* insiemi di descrittori */

	/* VARIABILI LIVELLO APPLICAZIONE */
	char buf[BUFSIZE];
	pct_r richiesta;
	pct_l login;
	Utente *listaUtenti = NULL;
	FILE *dbf;
	Utente *u_tmp = NULL;
	Negozio *n_tmp = NULL;
	Prodotto *p_tmp = NULL;
	Negozio n_buf;
	Prodotto p_buf;
	
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
	
	/* impostazioni socket */
	SetSockOpt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
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
	
	maxd = listenfd; /* maxd è il valore massimo dei descrittori in uso */
	
	/* il vettore client contiene i descrittori dei socket connessi */
	for (i = 0; i < 2; i++)
		server[i] = -1; /* inizializza il vettore di client */
	
	FD_ZERO(&active_fd_set); /* inizializza a zero l'insieme dei descrittori */
	FD_SET(listenfd, &active_fd_set); /* aggiunge il descrittore di ascolto */
	
		/* esecuzione server */
	while (1) {
		read_fd_set = active_fd_set; /* imposta il set di descrittori per la lettura */
		ready = Select(maxd + 1, &read_fd_set, NULL, NULL, NULL);
		
		/* Se è arrivata una richiesta di connessione, il socket di ascolto
		è leggibile: viene invocata accept() e creato un socket di connessione */
		if (FD_ISSET(listenfd, &read_fd_set)) {
			/* accettazione richiesta di connessione */
			connfd = Accept(listenfd, &cliaddr, &len);
			for (i = 0; i < 2; i++)
				if (server[i] < 0) {
					server[i] = connfd;
					break;
				}
			Read(connfd, buf, 1)) {	/* identifica il server */
			if (i == 2) {	/* errore se non ci sono posti liberi nel vettore client */
				fputs("accept: servers array size full\n", stderr);
				exit(1);
			}
			if (!strncmp(buf, "N", 1)) {
				Write(connfd, "0", 1);
				fputs("Connessione con serverN.\n", stdout);
			}
			else if (!strncmp(buf, "C", 1)) {
				Write(connfd, "0", 1);
				fputs("Connessione con serverC.\n", stdout);
			}
			else {
				fputs("Errore: connessione non riconosciuta.\n", stderr);
				for (i = 0; i < 2; i++)
					if (server[i] == connfd) {
						server[i] = -1;
						break;
					}
				close(connfd);
			}
			
			if (!strncmp(buf, "N", 1) || !strncmp(buf, "C", 1)) {
				/* inserisce connfd tra i descrittori da controllare ed aggiorna maxd */
				FD_SET(connfd, &active_fd_set);
				if (connfd > maxd)
					maxd = connfd;
				if (i > maxi)
					maxi = i;
				--ready; /* richiesta accettata/rifiutata, un socket in meno */
			}
		}
		
		/* controlla i socket attivi per controllare se sono leggibili */
		for (i = 0; i <= maxi && ready > 0; i++) {
			if ((sockfd = server[i]) >= 0) { /* se il descrittore non è stato selezionato, viene saltato */
				if (FD_ISSET(sockfd, &read_fd_set)) {
					if (!Read(connfd, &richiesta, sizeof(richiesta))) {
						if (richiesta.type == 'N')
							switch (richiesta.rich) {
								case '1':	/* Crea un nuovo negozio virtuale */
									u_tmp = ricercaUtente(listaUtenti, richiesta.query.q_user);		/* seleziona l'utente */
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
									u_tmp = ricercaUtente(listaUtenti, richiesta.query.q_user);		/* seleziona l'utente */
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
									u_tmp = ricercaUtente(listaUtenti, richiesta.query.q_user);		/* seleziona l'utente */
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
									u_tmp = ricercaUtente(listaUtenti, richiesta.query.q_user);		/* seleziona l'utente */
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
									u_tmp = ricercaUtente(listaUtenti, richiesta.query.q_user);
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
									u_tmp = ricercaUtente(listaUtenti, richiesta.query.q_user);
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
						else
							if (richiesta.type = 'C')
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
										
										break;
								}
					}
					else {
						/* chiusura connessione da parte del client */
						fputs("Connessione chiusa da client.\n", stdout);
						FD_CLR(sockfd, &active_fd_set); /* rimuove sockfd dalla lista dei socket da controllare */
						server[i] = -1; /* cancella sockfd da client */
					}
					--ready; /* richiesta elaborata o connessione chiusa, un socket in meno */
				}
			}
		}
	}
	exit(0);
}