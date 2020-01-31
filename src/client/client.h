#include "../../include/wrapper.h"
#include "../../include/pct_c.h"
#include "../../include/lista.h"
#include "acquisto.h"

void printLogo();
char menu();
void visualizzaNegozi(int sockfd);
void visualizzaProdottiNeg(int sockfd);
Acquisto *cercaProdotto(int sockfd, Acquisto *carrello);
Acquisto *rimuoviProdotto(Acquisto *carrello);