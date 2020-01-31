#ifndef THREADC_H
#define THREADC_H

#include "serverM.h"

void visualizzaNegozi_C(int connfd, Utente *listaUtenti, pct_c richiesta);
void visualizzaProdottiNeg_C(int connfd, Utente *listaUtenti, pct_c richiesta);
void cercaProdotto(int connfd, Utente *listaUtenti, pct_c richiesta);
#endif