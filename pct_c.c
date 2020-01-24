#include <stdio.h>

typedef struct QUERY_C {
	char q_neg[50];
	char q_prod[50];
	char q_prop[50];
}query_c;

typedef struct PCT_C {
	char rich;
	query_c query;
}pct_c;

/* 	tipi di richieste:
 *	1: riceve l'elenco dei negozi virtuali -- FATTO
 *	2: riceve l'elenco dei prodotti di un negozio virtuale 
 *	3: ricerca un prodotto in un negozio virtuale
 */
 
/*	query: utilizzato per la ricerca di un prodotto in un negozio virtuale */