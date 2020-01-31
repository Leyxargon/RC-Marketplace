#ifndef THREADN_H
#define THREADN_H

#include "serverM.h"

void creaNegozio_N(int connfd, Utente *listaUtenti, pct_n richiesta, FILE *dbf);
void rimuoviNegozio_N(int connfd, Utente *listaUtenti, pct_n richiesta, FILE *dbf);
void creaProdotto_N(int connfd, Utente *listaUtenti, pct_n richiesta, FILE *dbf);
void rimuoviProdotto_N(int connfd, Utente *listaUtenti, pct_n richiesta, FILE *dbf);
void visualizzaNegozi_N(int connfd, Utente *listaUtenti, pct_n richiesta, FILE *dbf);
void visualizzaProdottiNeg_N(int connfd, Utente *listaUtenti, pct_n richiesta, FILE *dbf);
Utente *registraUtente_N(int connfd, Utente *listaUtenti, pct_n richiesta, FILE *dbf);
void accessoUtente_N(int connfd, Utente *listaUtenti, pct_n richiesta, FILE *dbf);

#endif