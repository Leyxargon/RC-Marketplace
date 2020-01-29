#ifndef PCT_R_H
#define PCT_R_H

typedef struct QUERY_R {
	char q_user[50];
	char q_neg[50];
	char q_prod[50];
}query_r;

typedef struct PCT_R {
	char rich;
	char type;
	query_r query;
}pct_r;

/*	richieste di tipo N:
 *	1: crea un nuovo negozio virtuale
 *	2: elimina un negozio virtuale
 *	3: aggiungi un prodotto in un negozio virtuale
 *	4: rimuovi un prodotto in un negozio virtuale
 *	5: visualizza negozi virtuali
 *	6: visualizza prodotti di un negozio virtuale
 *	9: richiesta di login
 */
 
/*	richieste di tipo C:
 *	1: riceve l'elenco dei negozi virtuali
 *	2: riceve l'elenco dei prodotti di un negozio virtuale 
 *	3: ricerca un prodotto in un negozio virtuale
 */

#endif
