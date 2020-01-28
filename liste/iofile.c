#include "iofile.h"

Utente *importaDati(FILE *fdb) {
	char username[50];
	char password[50];
	char negozio[50];
	char prodotto[50];
	char buf[100];
	Utente *top = NULL;
	Utente *user_tmp = NULL;
	Negozio *neg_tmp = NULL;

	while (fscanf(fdb, "%s %s", username, password) == 2) {
		top = inserisciUtente(top, username, password);
		user_tmp = ricercaUtente(top, username);
		while (fgets(buf, sizeof(buf), fdb) != NULL) {
			if (buf[0] != '\n') {
                buf[strcspn(buf, "\r\n")] = '\0';
				if (!strncmp(buf, ".", 1)) {
					strcpy(negozio, &buf[1]);
					user_tmp -> negozi = inserisciNegozio(user_tmp -> negozi, negozio);
					neg_tmp = ricercaNegozio(user_tmp -> negozi, negozio);
				}
				else {
					if (!strncmp(buf, "_", 1)) {
						strcpy(prodotto, &buf[1]);
						neg_tmp -> prodotti = inserisciProdotto(neg_tmp -> prodotti, prodotto);
					} else
					if (!strncmp(buf, "&", 1))
						break;
				}
			}
		}
		memset(buf, 0, sizeof(buf));
	}
	return top;
}

void esportaDati(FILE *dbf, Utente *top) {
	Utente *user_tmp = top;
	Negozio *neg_tmp = NULL;
	Prodotto *prod_tmp = NULL;

	while (user_tmp != NULL) {
		fprintf(dbf, "%s %s\n", user_tmp -> username, user_tmp -> password);
		neg_tmp = user_tmp -> negozi;
		while (neg_tmp != NULL) {
			fprintf(dbf, ".%s\n", neg_tmp -> nome_negozio);
			prod_tmp = neg_tmp -> prodotti;
			while (prod_tmp != NULL) {
				fprintf(dbf, "_%s\n", prod_tmp -> nome_prodotto);
				prod_tmp = prod_tmp -> next;
			}
			neg_tmp = neg_tmp -> next;
		}
		user_tmp = user_tmp -> next;
		if (user_tmp != NULL)
			fprintf(dbf, "&\n");
	}
}
