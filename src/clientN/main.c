#include "clientN.h"

int main(int argc, char **argv) {
	/* gestione errore */
	if (argc != 2) {
		fprintf(stderr,"uso: %s <IP serverN>\n",argv[0]);
		exit(1);
	}

	int sockfd;
	struct sockaddr_in servaddr;
	int sc, i;
	char buf[BUFSIZE], user[50];
	pct_n richiesta;

	/* creazione socket */
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8001);

	/* lettura indirizzo IP */
	struct hostent *cliaddr;
	char *addr, **alias;
	cliaddr = GetHostByName(argv[1]);
    alias = cliaddr -> h_addr_list;	/* legge gli indirizzi IP dall'eventuale indirizzo simbolico */
	addr = (char *) inet_ntop(cliaddr -> h_addrtype, *alias, buf, sizeof(buf));
	Inet_pton(AF_INET, addr, &servaddr.sin_addr);

	fputs("Connessione con il serverN in corso...\n", stdout);
	/* connessione al server */
	Connect(sockfd, &servaddr);
	fputs("Connesso con serverN.\n", stdout);

	startScreen(sockfd, user);

	while (1) {
		sc = menu(user);
		switch (sc) {
			case '1':
				insNegozio(sockfd, user);
				break;
			case '2':
				cancNegozio(sockfd, user);
				break;
			case '3':
				insProdotto(sockfd, user);
				break;
			case '4':
				cancProdotto(sockfd, user);
				break;
			case '5':
				visualizzaNegozi(sockfd, user);
				break;
			case '6':
				visualizzaProdottiNeg(sockfd, user);
				break;
			default:
				exit(0);
		}
		fputs("Premere un tasto per continuare...", stdout);
		getchar();
	}
	exit(0);
}
