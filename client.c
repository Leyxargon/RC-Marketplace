#include "wrapper.h"
#include "pct_c.h"
#include "liste/lista.h"
#include "liste/iofile.h"

int main(int argc, char **argv) {
	int sockfd, n;
	char buf[BUFSIZE], buff[INET6_ADDRSTRLEN];
	int sc, i;
	struct sockaddr_in servaddr;
	pct_c richiesta;
	Negozio n_buf;
	Prodotto p_buf;

	
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
		fputs("Benvenuto, scegli cosa fare\n", stdout);
		fputs("1. Ricevere l'elenco dei negozi virtuali\n", stdout);
		fputs("2. Ricevere l'elenco dei prodotti di un negozio virtuale \n", stdout);
		fputs("3. Cercare un prodotto in un negozio virtuale\n", stdout);
		fscanf(stdin, "%d", &sc);
		if (sc == 0) {
			close(sockfd);
			break;
		}
		switch (sc) {
			case 1:
				getchar();
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
			case 2:
				getchar();
				richiesta.rich = '2';
				fputs("Inserire il nome del negozio: ", stdout);
				fgets(buf, sizeof(buf), stdin);
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
				
				break;
				
			case 3:
				getchar();
				richiesta.rich = '3';
				fputs("Inserire il nome del negozio: ", stdout);
				fgets(buf, sizeof(buf), stdin);
				buf[strcspn(buf, "\r\n")] = '\0';
				strcpy(richiesta.query.q_neg, buf);
				fputs("Inserire il nome del prodotto: ", stdout);
				fgets(buf, sizeof(buf), stdin);
				buf[strcspn(buf, "\r\n")] = '\0';
				strcpy(richiesta.query.q_prod, buf);
				Write(sockfd, &richiesta, sizeof(richiesta));
				fputs("Richiesta inviata, attendere prego...\n", stdout);
				Read(sockfd, buf, 1);
				if(buf[0]=='0')
					fputs("Prodotto presente\n", stdout);
				else
					fputs("Prodotto non presente\n", stdout);

				break;
		}
	}
	
	
	
		/*fputs("Connesso al serverC\n", stdout);
		fputs("Benvenuto, scegli cosa fare\n", stdout);
		fputs("1. Ricevere l'elenco dei negozi virtuali\n", stdout);
		fputs("2. Ricevere l'elenco dei prodotti di un negozio virtuale \n", stdout);
		fputs("3. Cercare un prodotto in un negozio virtuale\n", stdout);
		do{
			fgets(buf, sizeof(buf), stdin);
		}while(buf[0] != '1' && buf[0] != '2' && buf[3] != '3');
		pct_c richiesta;
		if(buf[0] == '1'){
			richiesta.rich='1';
			fputs("Inserire nome negozio: \n", stdout);
			fgets(richiesta.query.q_neg, sizeof(richiesta.query.q_neg), stdin);
			Write(sockfd, &richiesta, sizeof(richiesta));
			do {
				Read(sockfd, buf, sizeof(buf));
				if (buf[0] != '0')
					fprintf(stdout, "%s\n", buf);
			} while (buf[0] != '0');
		}
		else if(buf[0] == '2'){
			richiesta.rich='2';
			fputs("Inserire nome Proprietario Negozio: \n", stdout);
			fgets(richiesta.query.q_prop, sizeof(richiesta.query.q_prop), stdin);
			fputs("Inserire nome negozio: \n", stdout);
			fgets(richiesta.query.q_neg, sizeof(richiesta.query.q_neg), stdin);
			Write(sockfd, &richiesta, sizeof(richiesta));
      do {
        Read(sockfd, buf, sizeof(buf));
        if (buf[0] != '0' && buf[0]!='1')
          fprintf(stdout, "%s\n", buf);
      } while (buf[0] != '0' && buf[0]!='1');
      if(buf[0]=='1'){
        fputs("Nessun Risultato\n", stderr);
        }
	}
		else if(buf[0] == '3'){
			richiesta.rich='3';
			fputs("Inserire nome Proprietario Negozio: \n", stdout);
			fgets(richiesta.query.q_prop, sizeof(richiesta.query.q_prop), stdin);
			fputs("Inserire nome negozio: \n", stdout);
			fgets(richiesta.query.q_neg, sizeof(richiesta.query.q_neg), stdin);
			fputs("Inserire nome prodotto da cercare: \n", stdout);
			fgets(richiesta.query.q_prod, sizeof(richiesta.query.q_prod), stdin); 
			Write(sockfd, &richiesta, sizeof(richiesta));
      do {
        Read(sockfd, buf, sizeof(buf));
        if (buf[0] != '0' && buf[0]!='1')
          fprintf(stdout, "%s\n", buf);
      } while (buf[0] != '0' && buf[0]!='1');
      if(buf[0]=='1')
        fputs("Nessun Risultato\n", stderr);
      else {
        if(buf[0]=='0'){
        fputs("Prodotto trovato nel negozio\n", stderr);
        }
      }
    
  }
	else {
		fputs("Connessione fallita\n", stderr);
		exit(1);
	}*/

	exit(0);
}
