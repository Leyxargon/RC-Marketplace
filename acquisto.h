#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 50

typedef struct listaAcq {
	char prodotto [MAX];
	char negozio [MAX];
	struct listaAcq *next;
}Acquisto;

Acquisto *nuovoAcquisto(char prod[], char neg[]);
Acquisto *inserisciAcquisto(Acquisto *top, char prod[], char neg[]);
Acquisto *eliminaAcquisto(Acquisto *top, char prod[]);
Acquisto *ricercaAcquisto (Acquisto *top, char nomeprodotto[]);
void stampaAcquisti(Acquisto *top);
