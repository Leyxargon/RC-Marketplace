#include "wrapper.h"
#include "pct_n.h"
#include "liste/lista.h"
#include "login.h"

int main(int argc, char **argv) {
	int sockfd, sc, i;
	char buf[BUFSIZE], user[50];
	struct sockaddr_in servaddr;
	pct_n richiesta;
	
	/* gestione errore */
	if (argc != 2) {
		fprintf(stderr,"uso: %s <IPaddress>\n",argv[0]);
		exit(1);
	}
	
	/* creazione socket */
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8001);
	
	/* lettura indirizzo IP */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
		fprintf(stderr,"Errore inet_pton per %s\n", argv[1]);
		exit(1);
	}
	
	fputs("Connessione con il serverN in corso...\n", stdout);
	/* connessione al server */
	Connect(sockfd, &servaddr);
	fputs("Connesso con serverN.\n", stdout);
	
	user[0] = '\0';
	do {
		clear();
		do {
			printLogo();
			fputs("Benvenuto.\n", stdout);
			fputs("Selezionare operazione:\n", stdout);
			fputs("1) Accedi\n", stdout);
			fputs("2) Registrati\n", stdout);
			fputs("0) Esci\n", stdout);
			sc = getchar();
		} while (sc < '0' || sc > '2');
		clear();
		getchar();
		switch (sc) {
			case '1':
				login(user, sockfd);
				break;
			case '2':
				signup(sockfd);
				break;
			default:
				exit(0);
		}
	} while (user[0] == '\0');
	
	while (1) {
		do {
			clear();
			printLogo();
			fprintf(stdout, "Benvenuto %s!\n", user);
			fputs("Selezionare operazione:\n", stdout);
			fputs("1) Inserire un nuovo negozio\n", stdout);
			fputs("2) Rimuovere un negozio\n", stdout);
			fputs("3) Inserire un prodotto in un negozio\n", stdout);
			fputs("4) Rimuovere un prodotto in un negozio\n", stdout);
			fputs("5) Visualizzare i tuoi negozi\n", stdout);
			fputs("6) Visualizzare i prodotti di un tuo negozio\n", stdout);
			fputs("0) Esci\n", stdout);
			sc = getchar();
		} while (sc < '0' || sc > '6');
		getchar();
		clear();
		switch (sc) {
			case '1':
				richiesta.rich = '1';
				strcpy(richiesta.user, user);
				fputs("Inserire il nome del negozio: ", stdout);
				fgets(buf, sizeof(buf), stdin);
				if (buf[0] != '\n') {
					buf[strcspn(buf, "\r\n")] = '\0';
					strcpy(richiesta.query.q_neg, buf);
					fprintf(stdout, "Richiesto inserimento di %s\n", richiesta.query.q_neg);
					Write(sockfd, &richiesta, sizeof(richiesta));
					fputs("Richiesta inviata, attendere prego...\n", stdout);
					Read(sockfd, &buf, 1);
					if (buf[0] == '0')
						fputs("Operazione completata con successo.\n", stdout);
					else
						fputs("Operazione fallita.\n", stderr);
				}
				
				break;
			case '2':
				richiesta.rich = '2';
				strcpy(richiesta.user, user);
				fputs("Inserire il nome del negozio: ", stdout);
				fgets(buf, sizeof(buf), stdin);
				if (buf[0] != '\n') {
					buf[strcspn(buf, "\r\n")] = '\0';
					strcpy(richiesta.query.q_neg, buf);
					fprintf(stdout, "Richiesto cancellazione di %s\n", richiesta.query.q_neg);
					Write(sockfd, &richiesta, sizeof(richiesta));
					fputs("Richiesta inviata, attendere prego...\n", stdout);
					Read(sockfd, &buf, 1);
					if (buf[0] == '0')
						fputs("Operazione completata con successo.\n", stdout);
					else
						fputs("Operazione fallita.\n", stderr);
				}
				
				break;
			case '3':
				richiesta.rich = '3';
				strcpy(richiesta.user, user);
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
						fprintf(stdout, "Inserimento prodotto %s in %s.\n", richiesta.query.q_prod, richiesta.query.q_neg);
						Write(sockfd, &richiesta, sizeof(richiesta));
						fputs("Richiesta inviata, attendere prego...\n", stdout);
						Read(sockfd, &buf, 1);
						if (buf[0] == '0')
							fputs("Operazione completata con successo.\n", stdout);
						else
							fputs("Operazione fallita.\n", stderr);
					}
				}
				
				break;
			case '4':
				richiesta.rich = '4';
				strcpy(richiesta.user, user);
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
						fprintf(stdout, "Rimozione prodotto %s in %s.\n", richiesta.query.q_prod, richiesta.query.q_neg);
						Write(sockfd, &richiesta, sizeof(richiesta));
						fputs("Richiesta inviata, attendere prego...\n", stdout);
						Read(sockfd, &buf, 1);
						if (buf[0] == '0')
							fputs("Operazione completata con successo.\n", stdout);
						else
							fputs("Operazione fallita.\n", stderr);
					}
				}
				
				break;
			case '5':
				richiesta.rich = '5';
				strcpy(richiesta.user, user);
				Write(sockfd, &richiesta, sizeof(richiesta));
				clear();
				sprintf(buf, "NEGOZI DI %s\n", user);
				fputs(buf, stdout);
				for (i = 0; i < strlen(buf) - 1; i++)
					fputc('=', stdout);
				fputc('\n', stdout);
				do {
					Read(sockfd, &buf, sizeof(buf));
					if (buf[0] != '\0')
						fprintf(stdout, "%s\n", buf);
				} while (buf[0] != '\0');
				
				break;
			case '6':
				richiesta.rich = '6';
				strcpy(richiesta.user, user);
				fputs("Inserire il nome del negozio: ", stdout);
				fgets(buf, sizeof(buf), stdin);
				if (buf[0] != '\n') {
					buf[strcspn(buf, "\r\n")] = '\0';
					strcpy(richiesta.query.q_neg, buf);
					Write(sockfd, &richiesta, sizeof(richiesta));
					clear();
					sprintf(buf, "PRODOTTI DEL NEGOZIO %s DI %s\n", richiesta.query.q_neg, user);
					fputs(buf, stdout);
					for (i = 0; i < strlen(buf) - 1; i++)
						fputc('=', stdout);
					fputc('\n', stdout);
					do {
						Read(sockfd, &buf, sizeof(buf));
						if (buf[0] != '\0')
							fprintf(stdout, "%s\n", buf);
					} while (buf[0] != '\0');
				}
				
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
