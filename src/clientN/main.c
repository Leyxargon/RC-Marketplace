#include "clientN.h"

int main(int argc, char **argv) {
	int opt, portN = 8001;
	char address[200];
	address[0] = '\0';
	if (argc < 3 || argc > 5)
		printUsage(argv[0]);
	else {
		while ((opt = getopt(argc, argv, "i:p:")) != -1) {
			switch (opt) {
				case 'i':
					strcpy(address, optarg);
					break;
				case 'p':
					if ((portN = atoi(optarg)) <= 0)
						printUsage(argv[0]);
					break;
				case ':':
				case '?':
					printUsage(argv[0]);
			}
		}
		if (address[0] == '\0')
			printUsage(argv[0]);
	}

	int sockfd;
	struct sockaddr_in servaddr;
	int sc;
	char buf[BUFSIZE], user[50];

	/* creazione socket */
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(portN);

	/* lettura indirizzo IP */
	struct hostent *cliaddr;
	char *addr, **alias;
	cliaddr = GetHostByName(address);
    alias = cliaddr -> h_addr_list;	/* legge gli indirizzi IP dall'eventuale indirizzo simbolico */
	addr = (char *) inet_ntop(cliaddr -> h_addrtype, *alias, buf, sizeof(buf));
	Inet_pton(AF_INET, addr, &servaddr.sin_addr);

	fputs("Connessione con il serverN in corso...\n", stdout);
	/* connessione al server */
	Connect(sockfd, &servaddr);
	Write(sockfd, "n", 1);	/* identificazione al server */
	Read(sockfd, buf, 1);	/* lettura esito connessione */
	if (buf[0] == '0')
		fputs("Connesso con serverN.\n", stdout);
	else {
		fputs("Connessione fallita: server non valido.\n", stderr);
		exit(1);
	}

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
