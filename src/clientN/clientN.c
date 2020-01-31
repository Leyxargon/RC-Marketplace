#include "clientN.h"

void printLogo () {
	fputs("*****************************\n", stdout);
	fputs("******** MARKETPLACE ********\n", stdout);
	fputs("*****************************\n", stdout);
	fputs("+++++ Client negoziante +++++\n", stdout);
	fputs("_____________________________\n", stdout);
}

void signup(int sfd) {
	pct_n pacchetto;
	pct_l signup;
	char buf[BUFSIZE], ris;
	pacchetto.rich = '8';
	
	do {
		clear();
		printLogo();
		fputs("REGISTRAZIONE\n", stdout);
		fputs("Username: ", stdout);
		fgets(buf, sizeof(buf), stdin);
		buf[strcspn(buf, "\r\n")] = '\0';
		strcpy(signup.user, buf);
		fputs("Password: ", stdout);
		fgets(buf, sizeof(buf), stdin);
		buf[strcspn(buf, "\r\n")] = '\0';
		strcpy(signup.pass, buf);
		Write(sfd, &pacchetto, sizeof(pacchetto));	/* invio pacchetto di richiesta (tipo 5) */
		Read(sfd, &buf, 1);		/* lettura esito */
		Write(sfd, &signup, sizeof(signup));		/* invio pacchetto di login */
		fputs("Attendere, prego...\n", stdout);
		if (Read(sfd, &ris, 1)) {
			fputs("Errore di connessione.\n", stderr);
			exit(1);
		}
		if (ris == '0'){
			fprintf(stdout, "Registrazione effettuata con successo, benvenuto %s!\n", signup.user);
			fputs("Per effettuare l'accesso recarsi alla schermata di login.\n", stdout);
		}
		else
			fputs("L'username scelto è già esistente. Riprovare con un altro username.\n", stderr);

		fputs("Premere un tasto per continuare...", stdout);
		getchar();
	} while (ris != '0');
}

void login(char *usr_login, int sfd) {
	pct_n pacchetto;
	pct_l login;
	char buf[BUFSIZE], ris;
	pacchetto.rich = '9';
	
	do {
		clear();
		printLogo();
		fputs("LOGIN\n", stdout);
		fputs("Username: ", stdout);
		fgets(buf, sizeof(buf), stdin);
		buf[strcspn(buf, "\r\n")] = '\0';
		strcpy(login.user, buf);
		fputs("Password: ", stdout);
		fgets(buf, sizeof(buf), stdin);
		buf[strcspn(buf, "\r\n")] = '\0';
		strcpy(login.pass, buf);
		Write(sfd, &pacchetto, sizeof(pacchetto));	/* invio pacchetto di richiesta (tipo 5) */
		Read(sfd, &buf, 1);		/* lettura esito */
		Write(sfd, &login, sizeof(login));		/* invio pacchetto di login */
		fputs("Accesso in corso...\n", stdout);
		if (Read(sfd, &ris, 1)) {
			fputs("Errore di connessione.\n", stderr);
			exit(1);
		}
		if (ris == '0')
			fprintf(stdout, "Accesso effettuato come %s.\n", login.user);
		else {
			if (ris == '1')
				fputs("Utente non valido. Riprovare.\n", stderr);
			else
				fputs("Password non valida. Riprovare.\n", stderr);
			fputs("Premere un tasto per continuare...", stdout);
			getchar();
		}
	} while (ris != '0');
	
	strcpy(usr_login, login.user);
}

void startScreen(int sockfd, char *usr_login) {
	char sc;
	
	usr_login[0] = '\0';
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
				login(usr_login, sockfd);
				break;
			case '2':
				signup(sockfd);
				break;
			default:
				exit(0);
		}
	} while (usr_login[0] == '\0');
}

char menu(char *user) {
	char sc;
	
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
	
	return sc;
}

void insNegozio(int sockfd, char *user) {
	pct_n richiesta;
	char buf[BUFSIZE];
	
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
}

void cancNegozio(int sockfd, char *user) {
	pct_n richiesta;
	char buf[BUFSIZE];
	
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
}

void insProdotto(int sockfd, char *user) {
	pct_n richiesta;
	char buf[BUFSIZE];
	
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
			else {
				if (buf[0] == '1')
					fputs("Operazione fallita: il prodotto e' gia' stato inserito.\n", stderr);
				else
					fputs("Operazione fallita: il negozio selezionato non esiste.\n", stderr);
			}
		}
	}
}

void cancProdotto(int sockfd, char *user) {
	pct_n richiesta;
	char buf[BUFSIZE];
	
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
			else {
				if (buf[0] == '1')
					fputs("Operazione fallita: il prodotto e' gia' stato inserito.\n", stderr);
				else
					fputs("Operazione fallita: il negozio selezionato non esiste.\n", stderr);
			}
		}
	}
}

void visualizzaNegozi(int sockfd, char *user) {
	pct_n richiesta;
	char buf[BUFSIZE];
	int i;
	
	richiesta.rich = '5';
	strcpy(richiesta.user, user);
	Write(sockfd, &richiesta, sizeof(richiesta));
	clear();
	sprintf(buf, "NEGOZI DI %s\n", user);
	fputs(buf, stdout);
	for (i = 0; i < strlen(buf) - 1; i++)
		fputc('=', stdout);
	fputc('\n', stdout);
	i = 1;
	do {
		Read(sockfd, buf, sizeof(buf));
		if (buf[0] != '\0')
			fprintf(stdout, "%d) %s\n", i++, buf);
	} while (buf[0] != '\0');
	if (i == 1)
		fputs("Non e' presente alcun negozio.\n", stderr);
}

void visualizzaProdottiNeg(int sockfd, char *user) {
	pct_n richiesta;
	char buf[BUFSIZE];
	int i;
	
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
		i = 1;
		do {
			Read(sockfd, buf, sizeof(buf));
			if (buf[0] != '\0')
				fprintf(stdout, "%d) %s\n", i++, buf);
		} while (buf[0] != '\0');
		if (i == 1)
			fputs("Negozio vuoto o inesistente.\n", stderr);
	}
}