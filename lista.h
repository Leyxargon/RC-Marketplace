#ifndef LISTA_H
#define LISTA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 50

typedef struct prodotti {
	char nome_prodotto [MAX];
	struct prodotti *next;
}Prodotto;

typedef struct negozio {
	char nome_negozio [MAX];
	Prodotto *prodotti;
	struct negozio *next;
}Negozio;

typedef struct uid {
    char username [MAX];
    char password [MAX];
    Negozio *negozi;
    struct uid *next;
}Utente;

Utente *nuovoUtente(char User[MAX], char Psw[MAX]);
Negozio *nuovoNegozio(char Nom[]);
Prodotto *nuovoProdotto(char Nom_p[]);
Utente *inserisciUtente(Utente *top, char User[], char pwd[]);
Negozio *inserisciNegozio(Negozio *top, char nome_n[]);
Prodotto *inserisciProdotto(Prodotto *top, char nome_pr[]);
Utente *ricercaUtente (Utente *top, char nomeutente[]);
Negozio *ricercaNegozio (Negozio *top, char nomenegozio[]);
Prodotto *ricercaProdotto (Prodotto *top, char nomeprodotto[]);
Utente *eliminaUtente(Utente *top, char nome[]);
Negozio *eliminaNegozio(Negozio *top, char nome_negozio[]);
Prodotto *eliminaProdotto(Prodotto *top, char nome_prodotto[]);
Utente *eliminaUtenti(Utente *top);
Negozio *eliminaNegozi(Negozio *top);
Prodotto *eliminaProdotti(Prodotto *top);
void stampaListaUtenti(Utente *top);
void stampaListaNegozi(Negozio *top);
void stampaListaProdotti(Prodotto *top);
void visualizzaDati(Utente *top);

#endif