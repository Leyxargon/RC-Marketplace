#include <stdio.h>

typedef struct QUERY_N {
	char q_neg[50];
	char q_prod[50];
}query_n;

typedef struct PCT_N {
	char rich;
	char user[50];
	query_n query;
}pct_n;

/* 	tipi di richieste:
 *	1: crea un nuovo negozio virtuale
 *	2: elimina un negozio virtuale
 *	3: aggiungi un prodotto in un negozio virtuale
 *	4: rimuovi un prodotto in un negozio virtuale
 *	5: richiesta di login
 */

typedef struct LOGIN_N {
	char user[50];
	char pass[50];
}login_n;

/* pacchetto di richiesta login */
