#ifndef PCT_N_H
#define PCT_N_H

#include "pct_q.h"

typedef struct PCT_N {
	char rich;
	char user[50];
	pct_q query;
}pct_n;

/* 	tipi di richieste:
 *	1: crea un nuovo negozio virtuale
 *	2: elimina un negozio virtuale
 *	3: aggiungi un prodotto in un negozio virtuale
 *	4: rimuovi un prodotto in un negozio virtuale
 *	5: visualizza negozi virtuali
 *	6: visualizza prodotti di un negozio virtuale
 *	8: richiesta di registrazione
 *	9: richiesta di login
 */

#endif
