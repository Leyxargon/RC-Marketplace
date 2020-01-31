#include "threadC.h"

void visualizzaNegozi_C(int connfd, Utente *listaUtenti, pct_c richiesta) {
	Utente *u_tmp = listaUtenti;
	Negozio *n_tmp = NULL;
	Negozio n_buf;
	
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
}

void visualizzaProdottiNeg_C(int connfd, Utente *listaUtenti, pct_c richiesta) {
	Utente *u_tmp = listaUtenti;
	Negozio *n_tmp = NULL;
	Prodotto *p_tmp = NULL;
	Prodotto p_buf;
	
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
}

void cercaProdotto(int connfd, Utente *listaUtenti, pct_c richiesta) {
	Utente *u_tmp = listaUtenti;
	Negozio *n_tmp = NULL;
	
	while (u_tmp != NULL && n_tmp == NULL) { /* ricerca negozio */
		n_tmp = ricercaNegozio(u_tmp -> negozi, richiesta.query.q_neg);
		u_tmp = u_tmp -> next;
	}
	if (n_tmp != NULL) {
		if (ricercaProdotto(n_tmp -> prodotti, richiesta.query.q_prod))
			Write(connfd, "0", 1);	/* prodotto trovato */
		else
			Write(connfd, "1", 1);	/* prodotto non trovato */
	}
	else
		Write(connfd, "2", 1);	/* negozio inesistente */
}