#include "serverN.h"

void elaboraRichiesta(int masterfd, int sockfd, pct_n *richiesta) {
	char buf[BUFSIZE];
	pct_l credenz;
	
	switch (richiesta -> rich) {
		case '5':	/* VISUALIZZAZIONE NEGOZI */
		case '6':	/* VISUALIZZAZIONE PRODOTTI DI UN NEGOZIO */
			Write(masterfd, richiesta, sizeof(*richiesta));
			do {
				Read(masterfd, buf, sizeof(buf));
				Write(sockfd, buf, sizeof(buf));
			} while (buf[0] != '\0');

			break;
		case '8':	/* RICHIESTA DI REGISTRAZIONE */
		case '9':	/* RICHIESTA DI LOGIN */
			Write(sockfd, "0", 1);	/* invio pacchetto di conferma al client */
			Read(sockfd, &credenz, sizeof(credenz));	/* ricezione pacchetto di credenziali dal client */
			Write(masterfd, richiesta, sizeof(*richiesta));	/* invio pacchetto di richiesta al serverM */
			Read(masterfd, buf, 1);		/* lettura esito richiesta dal serverM */
			Write(masterfd, &credenz, sizeof(credenz));	/* invio pacchetto di login al serverM */
			Read(masterfd, buf, 1);
			Write(sockfd, buf, 1);

			break;
		default:	/* CASI 1, 2, 3, 4 */
			Write(masterfd, richiesta, sizeof(*richiesta));
			Read(masterfd, buf, 1);
			Write(sockfd, buf, 1);
			break;
	}
}