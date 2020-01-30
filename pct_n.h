#ifndef PCT_N_H
#define PCT_N_H

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
 *	5: visualizza negozi virtuali
 *	6: visualizza prodotti di un negozio virtuale
 *	9: richiesta di login
 */

typedef struct LOGIN_N {
	char user[50];
	char pass[50];
}login_n;

#endif
/* pacchetto di richiesta login */
