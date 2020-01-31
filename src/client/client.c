#include "client.h"

void printLogo() {
	fputs("*****************************\n", stdout);
	fputs("******** MARKETPLACE ********\n", stdout);
	fputs("*****************************\n", stdout);
	fputs("_____________________________\n", stdout);
}

char menu() {
	char sc;
	do {
		clear();
		printLogo();
		fputs("Benvenuto. Seleziona l'operazione da effettuare:\n", stdout);
		fputs("1) Ricevere l'elenco dei negozi virtuali\n", stdout);
		fputs("2) Ricevere l'elenco dei prodotti di un negozio virtuale \n", stdout);
		fputs("3) Cercare un prodotto in un negozio virtuale\n", stdout);
		fputs("4) Visualizzare il carrello\n", stdout);
		fputs("5) Eliminare un articolo dal carrello\n", stdout);
		fputs("0) Esci\n", stdout);
		sc = getchar();
	} while (sc < '0' || sc > '5');
	
	return sc;
}

void visualizzaNegozi(int sockfd) {
	int i = 1;
	Negozio n_buf;
	pct_c richiesta;
	
	richiesta.rich = '1';
	Write(sockfd, &richiesta, sizeof(richiesta));
	fputs("Richiesta inviata, attendere prego...\n", stdout);
	do {					
		Read(sockfd, &n_buf, sizeof(n_buf));
		if (n_buf.nome_negozio[0] != '\0')
			fprintf(stdout, "%d) %s \n", i++, n_buf.nome_negozio);
	} while (n_buf.nome_negozio[0] != '\0');
	if (i == 1)
		fputs("Nessun negozio disponibile.\n", stderr);
}

void visualizzaProdottiNeg(int sockfd) {
	int i = 1;
	char buf[BUFSIZE];
	Prodotto p_buf;
	pct_c richiesta;
	
	richiesta.rich = '2';
	fputs("Inserire il nome del negozio: ", stdout);
	fgets(buf, sizeof(buf), stdin);
	if (buf[0] != '\n') {
		buf[strcspn(buf, "\r\n")] = '\0';
		strcpy(richiesta.query.q_neg, buf);
		Write(sockfd, &richiesta, sizeof(richiesta));
		fputs("Richiesta inviata, attendere prego...\n", stdout);
		do {					
			Read(sockfd, &p_buf, sizeof(p_buf));
			if (p_buf.nome_prodotto[0] != '\0')
				fprintf(stdout, "%d) %s \n", i++, p_buf.nome_prodotto);
		} while (p_buf.nome_prodotto[0] != '\0');
		if (i == 1)
			fputs("Nessun prodotto disponibile o negozio inesistente.\n", stderr);
	}
}

Acquisto *cercaProdotto(int sockfd, Acquisto *carrello) {
	int i = 1;
	char buf[BUFSIZE], sc;
	Prodotto p_buf;
	pct_c richiesta;
	
	richiesta.rich = '3';
	fputs("Inserire il nome del negozio: ", stdout);
	fgets(buf, sizeof(buf), stdin);
	if (buf[0] != '\n') {
		buf[strcspn(buf, "\r\n")] = '\0';
		strcpy(richiesta.query.q_neg, buf);
		fputs("Inserire il nome del prodotto: ", stdout);
		fgets(buf, sizeof(buf), stdin);
		if (buf[0] != '\n') {
			buf[strcspn(buf, "\r\n")] = '\0';
			strcpy(richiesta.query.q_prod, buf);
			Write(sockfd, &richiesta, sizeof(richiesta));
			fputs("Richiesta inviata, attendere prego...\n", stdout);
			Read(sockfd, buf, 1);
			if (buf[0] == '0') {
				fputs("Prodotto presente\n", stdout);
				fputs("Inserire il prodotto nel carrello?\n", stdout);
				fputs("S) Si\nN) No\n", stdout);
				sc = getchar();
				if (sc == 'S' || sc == 's') {
					carrello = inserisciAcquisto(carrello, richiesta.query.q_prod, richiesta.query.q_neg);
					fputs("Prodotto inserito nel carrello.\n", stdout);
					getchar();
				}
			}
			else {
				if (buf[0] == '1')
					fputs("Prodotto non presente in negozio.\n", stderr);
				else
					fputs("Il negozio selezionato non esiste.\n", stderr);
			}
		}
	}
	return carrello;
}

Acquisto *rimuoviProdotto(Acquisto *carrello) {
	char buf[BUFSIZE];
	
	fputs("Inserire il nome del prodotto: ", stdout);
	fgets(buf, sizeof(buf), stdin);
	if (buf[0] != '\n') {
		buf[strcspn(buf, "\r\n")] = '\0';
		if (ricercaAcquisto(carrello, buf) != NULL){
			carrello = eliminaAcquisto(carrello, buf);
			fputs("Prodotto eliminato correttamente.\n", stdout);
			getchar();
		}
		else
			fputs("Prodotto non trovato \n", stderr);
	}
	return carrello;
}