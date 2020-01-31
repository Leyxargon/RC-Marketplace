#include <stdio.h>
#include <string.h>
#include "lista.h"

Utente *importaDati(FILE *dbf);
void commit(FILE *dbf, Utente *top);
