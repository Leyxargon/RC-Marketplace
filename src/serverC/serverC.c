#include "serverC.h"

void elaboraRichiesta(int masterfd, int sockfd, pct_c *richiesta) {
	char buf[BUFSIZE];
	Negozio n_buf;
	Prodotto p_buf;
	
	switch (richiesta -> rich) {
		case '1':
			Write(masterfd, richiesta, sizeof(*richiesta));
			do {
				Read(masterfd, &n_buf, sizeof(n_buf));
				Write(sockfd, &n_buf, sizeof(n_buf));
			} while (n_buf.nome_negozio[0] != '\0');
			break;

		case '2':
			Write(masterfd, richiesta, sizeof(*richiesta));
			do {
				Read(masterfd, &p_buf, sizeof(p_buf));
				Write(sockfd, &p_buf, sizeof(p_buf));
			} while (p_buf.nome_prodotto[0] != '\0');
			break;

		case '3':
			Write(masterfd, richiesta, sizeof(*richiesta));
			Read(masterfd, buf, 1);
			Write(sockfd, buf, 1);
			break;
	}
}

void printUsage(char *prog) {
	fprintf(stderr, "uso: %s -i <indirizzo> [opzione...]\n", prog);
	fputs("\t-i <indirizzo>\t\tIndirizzo serverM\n", stderr);
	fputs("\t-m <porta>\t\tNumero di porta serverM [default: 8000]\n", stderr);
	fputs("\t-p <porta>\t\tNumero di porta serverC [default: 8002]\n", stderr);
	exit(1);
}