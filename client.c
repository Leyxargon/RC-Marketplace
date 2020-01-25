#include "wrapper.h"
#include "pct_c.c"

int main(int argc, char **argv) {
	int sockfd, n;
	char buf[BUFSIZE], buff[INET6_ADDRSTRLEN];
	struct sockaddr_in servaddr;
	struct hostent *cliaddr;
	
	char **alias;
	char *addr;
	
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
	cliaddr = GetHostByName(argv[1]);
    alias = cliaddr->h_addr_list;	/* legge gli indirizzi IP dall'eventuale indirizzo simbolico */
	addr = (char *) inet_ntop(cliaddr->h_addrtype, *alias, buff, sizeof(buff));
	Inet_pton(AF_INET, addr, &servaddr.sin_addr);
	
	/* connessione al server */
  fputs("Connessione al serverC\n", stdout);
	Connect(sockfd, &servaddr);

	if (!strncmp(buf, "0", 1)){
		printf("Connesso al serverC\n");
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
  }
	else {
		fputs("Connessione fallita\n", stderr);
		exit(1);
	}

	exit(0);
}
