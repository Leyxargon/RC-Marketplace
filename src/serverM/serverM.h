#ifndef SERVERM_H
#define SERVERM_H

#include "../../include/lista.h"
#include "../../include/wrapper.h"
#include "../../include/iofile.h"
#include "../../include/pct_c.h"
#include "../../include/pct_n.h"
#include "../../include/pct_l.h"
#include <pthread.h>
#include "threadN.h"
#include "threadC.h"

typedef struct PTHREAD_ARGS {
    int connfd;
	FILE *dbf;
	Utente *listaUtenti;
}pthread_args;

void *serverC(void *connfd);
void *serverN(void *connfd);
void printUsage(char *prog);

#endif