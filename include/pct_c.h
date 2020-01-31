#ifndef PCT_C_H
#define PCT_C_H

#include "pct_q.h"

typedef struct PCT_C {
	char rich;
	pct_q query;
}pct_c;

#endif

/* 	tipi di richieste:
 *	1: riceve l'elenco dei negozi virtuali
 *	2: riceve l'elenco dei prodotti di un negozio virtuale 
 *	3: ricerca un prodotto in un negozio virtuale
 */
 
/*	query: utilizzato per la ricerca di un prodotto in un negozio virtuale */