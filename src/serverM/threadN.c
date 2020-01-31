#include "threadN.h"

void creaNegozio_N(int connfd, Utente *listaUtenti, pct_n richiesta, FILE *dbf) {
	Utente *u_tmp = ricercaUtente(listaUtenti, richiesta.user);		/* seleziona l'utente */
	if (ricercaNegozio(u_tmp -> negozi, richiesta.query.q_neg) == NULL) { /* assicura che il negozio non esista */
		u_tmp -> negozi = inserisciNegozio(u_tmp -> negozi, richiesta.query.q_neg);
		commit(dbf, listaUtenti);
		visualizzaDati(listaUtenti);
		Write(connfd, "0", 1);	/* operazione effettuata correttamente */
	}
	else
		Write(connfd, "1", 1);	/* negozio già esistente */
}

void rimuoviNegozio_N(int connfd, Utente *listaUtenti, pct_n richiesta, FILE *dbf) {
	Utente *u_tmp = ricercaUtente(listaUtenti, richiesta.user);		/* seleziona l'utente */
	if (ricercaNegozio(u_tmp -> negozi, richiesta.query.q_neg) != NULL) {  /* assicura che il negozio non esista */
		u_tmp -> negozi = eliminaNegozio(u_tmp -> negozi, richiesta.query.q_neg);
		commit(dbf, listaUtenti);
		visualizzaDati(listaUtenti);
		Write(connfd, "0", 1);	/* operazione effettuata correttamente */
	}
	else
		Write(connfd, "1", 1); /* negozio inesistente */
}

void creaProdotto_N(int connfd, Utente *listaUtenti, pct_n richiesta, FILE *dbf) {
	Utente *u_tmp = ricercaUtente(listaUtenti, richiesta.user);		/* seleziona l'utente */
	Negozio *n_tmp = NULL;
	
	if ((n_tmp = ricercaNegozio(u_tmp -> negozi, richiesta.query.q_neg)) != NULL) {
		if (ricercaProdotto(n_tmp -> prodotti, richiesta.query.q_prod) == NULL) {
			n_tmp -> prodotti = inserisciProdotto(n_tmp -> prodotti, richiesta.query.q_prod);
			commit(dbf, listaUtenti);
			visualizzaDati(listaUtenti);
			Write(connfd, "0", 1);	/* operazione effettuata correttamente */
		}
		else
			Write(connfd, "1", 1);	/* prodotto già esistente in quel negozio */
	}
	else
		Write(connfd, "2", 1);	/* negozio inesistente */
}

void rimuoviProdotto_N(int connfd, Utente *listaUtenti, pct_n richiesta, FILE *dbf) {
	Utente *u_tmp = ricercaUtente(listaUtenti, richiesta.user);		/* seleziona l'utente */
	Negozio *n_tmp = NULL;
	
	if ((n_tmp = ricercaNegozio(u_tmp -> negozi, richiesta.query.q_neg)) != NULL) {
		if (ricercaProdotto(n_tmp -> prodotti, richiesta.query.q_prod) != NULL) {
			n_tmp -> prodotti = eliminaProdotto(n_tmp -> prodotti, richiesta.query.q_prod);
			commit(dbf, listaUtenti);
			visualizzaDati(listaUtenti);
			Write(connfd, "0", 1);	/* operazione effettuata correttamente */
		}
		else
			Write(connfd, "1", 1); /* prodotto inesistente */
	}
	else
		Write(connfd, "2", 1); /* negozio inesistente */
}

void visualizzaNegozi_N(int connfd, Utente *listaUtenti, pct_n richiesta, FILE *dbf) {
	Utente *u_tmp = ricercaUtente(listaUtenti, richiesta.user);		/*seleziona l'utente */
	Negozio *n_tmp = u_tmp -> negozi;
	char buf[BUFSIZE];
	
	while (n_tmp != NULL) {		/* accesso ai negozi */
		sprintf(buf, "%s", n_tmp -> nome_negozio);
		Write(connfd, buf, sizeof(buf));
		n_tmp = n_tmp -> next;
	}
	buf[0] = '\0';	/* fine accesso ai negozi */
	Write(connfd, buf, sizeof(buf));
}

void visualizzaProdottiNeg_N(int connfd, Utente *listaUtenti, pct_n richiesta, FILE *dbf) {
	Utente *u_tmp = ricercaUtente(listaUtenti, richiesta.user);		/* seleziona l'utente */
	Negozio *n_tmp = NULL;
	Prodotto *p_tmp = NULL;
	char buf[BUFSIZE];
	
	if ((n_tmp = ricercaNegozio(u_tmp -> negozi, richiesta.query.q_neg)) != NULL) {
		p_tmp = n_tmp -> prodotti;
		while (p_tmp != NULL) {		/* accesso ai prodotti del negozio */
			sprintf(buf, "%s", p_tmp -> nome_prodotto);
			Write(connfd, buf, sizeof(buf));
			p_tmp = p_tmp -> next;
		}
	}
	buf[0] = '\0';	/* fine accesso ai prodotti */
	Write(connfd, buf, sizeof(buf));
}

Utente *registraUtente_N(int connfd, Utente *listaUtenti, pct_n richiesta, FILE *dbf) {
	Utente *u_tmp = listaUtenti;
	pct_l credenz;
	
	Write(connfd, "0", 1);	/* Richiesta di ricezione pacchetto di credenziali */
	Read(connfd, &credenz, sizeof(credenz));
	if (ricercaUtente(listaUtenti, credenz.user) == NULL) {
		u_tmp = inserisciUtente(listaUtenti, credenz.user, credenz.pass);
		commit(dbf, listaUtenti);
		visualizzaDati(listaUtenti);
		Write(connfd, "0", 1);	/* operazione effettuata correttamente */
	}
	else
		Write(connfd, "1", 1); /* utente già esistente */
	
	return u_tmp;
}

void accessoUtente_N(int connfd, Utente *listaUtenti, pct_n richiesta, FILE *dbf) {
	Utente *u_tmp = listaUtenti;
	pct_l credenz;
	
	Write(connfd, "0", 1);	/* Richiesta di ricezione pacchetto di credenziali */
	Read(connfd, &credenz, sizeof(credenz));
	if ((u_tmp = ricercaUtente(listaUtenti, credenz.user)) == NULL) 
		Write(connfd, "1", 1); /* utente non valido */
	else {
		if (!strcmp(u_tmp -> password, credenz.pass))
			Write(connfd, "0", 1);	/* accesso effettuato correttamente */
		else
			Write(connfd, "2", 1); /* password non valida */
	}
}