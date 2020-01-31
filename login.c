#include "login.h"

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

void printLogo () {
	fputs("*****************************\n", stdout);
	fputs("******** MARKETPLACE ********\n", stdout);
	fputs("*****************************\n", stdout);
	fputs("+++++ Client negoziante +++++\n", stdout);
	fputs("_____________________________\n", stdout);
}