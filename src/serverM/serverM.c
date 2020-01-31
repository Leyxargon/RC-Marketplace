#include "serverM.h"

void *serverC(void *args) {
	Utente *listaUtenti = ((pthread_args *) args) -> listaUtenti;
	int connfd = ((pthread_args *) args) -> connfd;
	pct_c richiesta;

	while (1) {
		if (!Read(connfd, &richiesta, sizeof(richiesta))) {
			switch(richiesta.rich) {
				case '1':	/* Visualizza l'elenco dei negozi virtuali */
					visualizzaNegozi_C(connfd, listaUtenti, richiesta);
					break;
				case '2':	/* Visualizza i prodotti di un negozio */
					visualizzaProdottiNeg_C(connfd, listaUtenti, richiesta);
					break;
				case '3':	/* Cerca un prodotto in un negozio */
					cercaProdotto(connfd, listaUtenti, richiesta);
					break;
				default:
					fputs("Richiesta non valida.\n", stderr);
					break;
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
	pct_n richiesta;

	while (1) {
		if (!Read(connfd, &richiesta, sizeof(richiesta))) {
			switch (richiesta.rich) {
				case '1':	/* Crea un nuovo negozio virtuale */
					creaNegozio_N(connfd, listaUtenti, richiesta, dbf);
					break;
				case '2':	/* Elimina un negozio virtuale */
					rimuoviNegozio_N(connfd, listaUtenti, richiesta, dbf);
					break;
				case '3':	/* Aggiungi un prodotto in un negozio virtuale */
					creaProdotto_N(connfd, listaUtenti, richiesta, dbf);
					break;
				case '4':	/* Rimuovi un prodotto in un negozio virtuale */
					rimuoviProdotto_N(connfd, listaUtenti, richiesta, dbf);
					break;
				case '5':	/* Visualizza i negozi virtuali di un negoziante */
					visualizzaNegozi_N(connfd, listaUtenti, richiesta, dbf);
					break;
				case '6':	/* Visualizza i prodotti di un negozio virtuale di un negoziante */
					visualizzaProdottiNeg_N(connfd, listaUtenti, richiesta, dbf);
					break;
				case '8':	/* Richiesta di registrazione */
					listaUtenti = registraUtente_N(connfd, listaUtenti, richiesta, dbf);
					break;
				case '9':	/* Richiesta di login */
					accessoUtente_N(connfd, listaUtenti, richiesta, dbf);
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
