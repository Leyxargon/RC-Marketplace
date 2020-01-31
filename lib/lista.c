#include "../include/lista.h"

Utente *nuovoUtente(char User[MAX], char Psw[MAX]) {
	Utente *pnt = (Utente *) malloc (sizeof(Utente));
	strcpy(pnt -> username, User);
	strcpy(pnt -> password, Psw);
    pnt -> negozi = NULL;
	pnt -> next = NULL;
	return pnt;
}

Negozio *nuovoNegozio(char Nom[]) {
	Negozio *pnt = (Negozio *) malloc (sizeof(Negozio));
    strcpy(pnt -> nome_negozio, Nom);
    pnt -> prodotti= NULL;
	pnt -> next = NULL;
	return pnt;
}

Prodotto *nuovoProdotto(char Nom_p[]) {
	Prodotto *pnt = (Prodotto *) malloc (sizeof(Prodotto));
	strcpy(pnt -> nome_prodotto, Nom_p);
	pnt -> next = NULL;
	return pnt;
}

Utente *inserisciUtente(Utente *top, char User[], char pwd[]) {
	if (top == NULL || strcmp(top -> username, User) > 0) {
		Utente *tmp = nuovoUtente(User, pwd);
		tmp -> next = top;
		top = tmp;
		return top;
	}
	else
		top -> next = inserisciUtente(top -> next, User, pwd);
}

Negozio *inserisciNegozio(Negozio *top, char nome_n[]) {
	if (top == NULL || strcmp(top -> nome_negozio, nome_n) > 0) {
		Negozio *tmp = nuovoNegozio(nome_n);
		tmp -> next = top;
		top = tmp;
		return top;
	}
	else
		top -> next = inserisciNegozio(top -> next, nome_n);
}

Prodotto *inserisciProdotto(Prodotto *top, char nome_pr[]) {
	if (top == NULL || strcmp(top -> nome_prodotto, nome_pr) > 0) {
		Prodotto *tmp = nuovoProdotto(nome_pr);
		tmp -> next = top;
		top = tmp;
		return top;
	}
	else
		top -> next = inserisciProdotto(top -> next, nome_pr);
}

Utente *ricercaUtente (Utente *top, char nomeutente[]){
	if (top == NULL || !strcmp(top->username, nomeutente))
		return top;
	else
		return ricercaUtente(top -> next, nomeutente);
}

Negozio *ricercaNegozio (Negozio *top, char nomenegozio[]){
	if (top == NULL || !strcmp(top->nome_negozio, nomenegozio))
		return top;
	else
		return ricercaNegozio(top -> next, nomenegozio);
}

Prodotto *ricercaProdotto (Prodotto *top, char nomeprodotto[]){
	if (top == NULL || !strcmp(top->nome_prodotto, nomeprodotto))
		return top;
	else
		return ricercaProdotto(top -> next, nomeprodotto);
}

Utente *eliminaUtente(Utente *top, char nome[]) {
	Utente *tmp;
	if (top != NULL) {
		if (!strcmp(top -> username, nome)) {
			eliminaNegozi(top -> negozi);
			tmp = top -> next;
			free(top);
			top = tmp;
		}
		else
			top -> next = eliminaUtente(top -> next, nome);
	}
	return top;
}

Negozio *eliminaNegozio(Negozio *top, char nome_negozio[]) {
	Negozio *tmp;
	if (top != NULL) {
		if (!strcmp(top -> nome_negozio, nome_negozio)) {
			eliminaProdotti(top -> prodotti);
			tmp = top -> next;
			free(top);
			top = tmp;
		}
		else
			top -> next = eliminaNegozio(top -> next, nome_negozio);
	}
	return top;
}

Prodotto *eliminaProdotto(Prodotto *top, char nome_prodotto[]) {
	Prodotto *tmp;
	if (top != NULL) {
		if (!strcmp(top -> nome_prodotto, nome_prodotto)) {
			tmp = top -> next;
			free(top);
			top = tmp;
		}
		else
			top -> next = eliminaProdotto(top -> next, nome_prodotto);
	}
	return top;
}

Utente *eliminaUtenti(Utente *top) {
	if (top != NULL) {
		eliminaNegozi(top -> negozi);
		eliminaUtenti(top -> next);
		free(top);
	}
	else
		return NULL;
}

Negozio *eliminaNegozi(Negozio *top) {
	if (top != NULL) {
		eliminaProdotti(top -> prodotti);
		eliminaNegozi(top -> next);
		free(top);
	}
	else
		return NULL;
}

Prodotto *eliminaProdotti(Prodotto *top) {
	if (top != NULL) {
		eliminaProdotti(top -> next);
		free(top);
	}
	else
		return NULL;
}

void stampaListaUtenti(Utente *top) {
	if (top != NULL) {
		printf("%s -> ", top -> username);
		stampaListaUtenti(top -> next);
	}
	else
		printf("NULL\n");
}

void stampaListaNegozi(Negozio *top) {
	if (top != NULL) {
		printf("%s -> ", top -> nome_negozio);
		stampaListaNegozi(top -> next);
	}
	else
		printf("NULL\n");
}

void stampaListaProdotti(Prodotto *top) {
	if (top != NULL) {
		printf("%s -> ", top -> nome_prodotto);
		stampaListaProdotti(top -> next);
	}
	else
		printf("NULL\n");
}

void visualizzaDati(Utente *top) {
	Utente *user_tmp = top;
	Negozio *neg_tmp = NULL;
	Prodotto *prod_tmp = NULL;

	while (user_tmp != NULL) {
		printf("Negozi di %s:\n", user_tmp -> username);
		neg_tmp = user_tmp -> negozi;
		while (neg_tmp != NULL) {
			printf("%s => ", neg_tmp -> nome_negozio);
			prod_tmp = neg_tmp -> prodotti;
			stampaListaProdotti(prod_tmp);
			neg_tmp = neg_tmp -> next;
		}
		printf("\n\n");
		user_tmp = user_tmp -> next;
	}
}
