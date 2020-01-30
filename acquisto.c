#include "acquisto.h"

Acquisto *nuovoAcquisto(char prod[], char neg[]) {
	Acquisto *pnt = (Acquisto *) malloc (sizeof(Acquisto));
	strcpy(pnt -> prodotto, prod);
	strcpy(pnt -> negozio, neg);
	pnt -> next = NULL;
	return pnt;
}

Acquisto *inserisciAcquisto(Acquisto *top, char prod[], char neg[]) {
	if (top == NULL) {
		Acquisto *tmp = nuovoAcquisto(prod, neg);
		tmp -> next = top;
		top = tmp;
		return top;
	}
	else
		top -> next = inserisciAcquisto(top -> next, prod, neg);
}

Acquisto *eliminaAcquisto(Acquisto *top, char prod[]) {
	Acquisto *tmp;
	if (top != NULL) {
		if (!strcmp(top -> prodotto, prod)) {
			tmp = top -> next;
			free(top);
			top = tmp;
		}
		else
			top -> next = eliminaAcquisto(top -> next, prod);
	}
	return top;
}

Acquisto *ricercaAcquisto (Acquisto *top, char nomeprodotto[]){
	if (top == NULL || !strcmp(top->prodotto, nomeprodotto))
		return top;
	else
		return ricercaAcquisto(top -> next, nomeprodotto);
}

void stampaAcquisti(Acquisto *top) {
	if (top == NULL)
		fputs("Il carrello e' vuoto.\n", stdout);
	else {
		Acquisto *tmp = top;
		int i = 1;
		while (tmp != NULL) {
			printf("%d) %s da %s\n", i++, tmp -> prodotto, tmp -> negozio);
			tmp = tmp -> next;
		}
	}
}