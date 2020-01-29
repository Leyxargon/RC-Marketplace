#include "acquisto.h"

Acquisto *nuovoAcquisto(char prod[], char neg[]) {
	Acquisto *pnt = (Acquisto *) malloc (sizeof(Acquisto));
	strcpy(pnt -> prodotto, prod);
	strcpy(pnt -> negozio, neg);
	pnt -> next = NULL;
	return pnt;
}

Acquisto *inserisciAcquisto(Acquisto *top, char prod[], char neg[]) {
	if (top == NULL || strcmp(top -> prodotto, prod) > 0) {
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
		if (strcmp(top -> prodotto, prod)==0) {
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
	if (top == NULL || strcmp(top->prodotto, nomeprodotto)==0)
		return top;
	else
		return ricercaAcquisto(top -> next, nomeprodotto);
}

void stampaAcquisti(Acquisto *top) {
	if (top != NULL) {
		printf("%s da %s -> ", top -> prodotto, top -> negozio);
		stampaAcquisti(top -> next);
	}
	else
		printf("NULL\n");
}