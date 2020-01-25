#include "wrapper.h"
#include "pct_n.c"
#include "liste/lista.h"
#include "liste/iofile.h"

int main(int argc, char **argv) {
	int sockfd, n;
	char buf[BUFSIZE];
	int sc;
	struct sockaddr_in servaddr;
	pct_n richiesta;
	
	/* gestione errore */
	if (argc != 2) {
		fprintf(stderr,"usage: %s <IPaddress>\n",argv[0]);
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
	while (1) {
		fputs("Premere 1 per inserire un nuovo negozio\n", stdout);
		fscanf(stdin, "%d", &sc);
		if (sc == 0) {
			close(sockfd);
			break;
		}
		switch (sc) {
			case 1:
				getchar();
				richiesta.rich = '1';
				fputs("Inserire il nome del negozio: ", stdout);
				fgets(buf, sizeof(buf), stdin);
				buf[strcspn(buf, "\r\n")] = '\0';
				strcpy(richiesta.user, "Amadeus");
				strcpy(richiesta.query.q_neg, buf);
				fprintf(stdout, "Richiesto inserimento di %s\n", richiesta.query.q_neg);
				Write(sockfd, &richiesta, sizeof(richiesta));
				fputs("Richiesta inviata, attendere prego...\n", stdout);
				Read(sockfd, &buf, 1);
				if (buf[0] == '0')
					fputs("Operazione completata con successo.\n", stdout);
				else
					fputs("Operazione fallita.\n", stderr);

				break;
		}
	}
	exit(0);
}