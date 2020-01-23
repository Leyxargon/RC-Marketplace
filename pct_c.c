#include <stdio.h>

typedef struct PCT_C {
	char rich;
	char query[50];
}pct_c;

/* 	tipi di richieste:
 *	1: riceve l'elenco dei negozi virtuali
 *	2: riceve l'elenco dei prodotti di un negozio virtuale
 *	3: ricerca un prodotto in un negozio virtuale
 */
 
/*	query: utilizzato per la ricerca di un prodotto in un negozio virtuale */