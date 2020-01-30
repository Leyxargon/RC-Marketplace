#include "wrapper.h"
#include "pct_c.h"
#include "liste/lista.h"
#include "libclient.c"
#include "acquisto.h"

int main(int argc, char **argv) {
	int sockfd, n;
	char buf[BUFSIZE], buff[INET6_ADDRSTRLEN];
	char sc;
	int i;
	struct sockaddr_in servaddr;
	pct_c richiesta;
	Negozio n_buf;
	Prodotto p_buf;
	Acquisto *carrello = NULL;

	
	/* gestione errore */
	if (argc != 2) {
		fprintf(stderr,"usage: %s <IPaddress>\n",argv[0]);
		exit(1);
	}
	
	/* creazione socket */
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8002);
	
	/* lettura indirizzo IP */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
		fprintf(stderr,"Errore inet_pton per %s\n", argv[1]);
		exit(1);
	}
	
	/* connessione al server */
	fputs("Connessione con il serverC in corso...\n", stdout);
	Connect(sockfd, &servaddr);
	fputs("Connesso con serverC.\n", stdout);
	while (1) {
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
		getchar();
		clear();
		switch (sc) {
			case '1':
				richiesta.rich = '1';
				Write(sockfd, &richiesta, sizeof(richiesta));
				fputs("Richiesta inviata, attendere prego...\n", stdout);
				i = 1;
				do {					
					Read(sockfd, &n_buf, sizeof(n_buf));
					if (n_buf.nome_negozio[0] != '\0')
						fprintf(stdout, "%d) %s \n", i++, n_buf.nome_negozio);
				} while (n_buf.nome_negozio[0] != '\0');
				
				break;
			case '2':
				richiesta.rich = '2';
				fputs("Inserire il nome del negozio: ", stdout);
				fgets(buf, sizeof(buf), stdin);
				if (buf[0] != '\n') {
					buf[strcspn(buf, "\r\n")] = '\0';
					strcpy(richiesta.query.q_neg, buf);
					Write(sockfd, &richiesta, sizeof(richiesta));
					fputs("Richiesta inviata, attendere prego...\n", stdout);
					i = 1;
					do {					
						Read(sockfd, &p_buf, sizeof(p_buf));
						if (p_buf.nome_prodotto[0] != '\0')
							fprintf(stdout, "%d) %s \n", i++, p_buf.nome_prodotto);
					} while (p_buf.nome_prodotto[0] != '\0');
				}
				
				break;
			case '3':
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
								fputs("Prodotto inserito nel carrello. \n", stdout);
								getchar();
							}
						}
						else
							fputs("Prodotto non presente\n", stdout);
					}
				}
				
				break;
			case '4':
				stampaAcquisti(carrello);
				break;
				
			case '5':
				fputs("Inserire il nome del prodotto: ", stdout);
                fgets(buf, sizeof(buf), stdin);
                buf[strcspn(buf, "\r\n")] = '\0';
				if(ricercaAcquisto(carrello, buf)!=NULL){
					carrello=eliminaAcquisto(carrello, buf);
					fputs("Prodotto eliminato correttamente \n", stdout);
					getchar();
				}
				else
					fputs("Prodotto non trovato \n", stdout);
				
				break;
				
			default:
				close(sockfd);
				exit(0);
		}
		fputs("Premere un tasto per continuare...", stdout);
		getchar();
	}
	exit(0);
}
