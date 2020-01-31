#include "client.h"

int main(int argc, char **argv) {
	/* gestione errore */
	if (argc != 2) {
		fprintf(stderr,"uso: %s <IP serverC>\n",argv[0]);
		exit(1);
	}
	
	int sockfd;
	struct sockaddr_in servaddr;
	int sc, i;
	char buf[BUFSIZE];
	pct_c richiesta;
	Acquisto *carrello = NULL;
	
	/* creazione socket */
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8002);
	
	/* lettura indirizzo IP */
	struct hostent *cliaddr;
	char *addr, **alias;
	cliaddr = GetHostByName(argv[1]);
	alias = cliaddr -> h_addr_list;	/* legge gli indirizzi IP dall'eventuale indirizzo simbolico */
	addr = (char *) inet_ntop(cliaddr -> h_addrtype, *alias, buf, sizeof(buf));
	Inet_pton(AF_INET, addr, &servaddr.sin_addr);
	
	/* connessione al server */
	fputs("Connessione con il serverC in corso...\n", stdout);
	Connect(sockfd, &servaddr);
	fputs("Connesso con serverC.\n", stdout);
	while (1) {
		sc = menu();
		getchar();
		clear();
		switch (sc) {
			case '1':
				visualizzaNegozi(sockfd);
				break;
			case '2':
				visualizzaProdottiNeg(sockfd);
				break;
			case '3':
				carrello = cercaProdotto(sockfd, carrello);
				break;
			case '4':
				stampaAcquisti(carrello);
				break;
			case '5':
				carrello = rimuoviProdotto(carrello);
				break;
			default:
				exit(0);
		}
		fputs("Premere un tasto per continuare...", stdout);
		getchar();
	}
	exit(0);
}
