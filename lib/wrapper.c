#include "../include/wrapper.h"

/**	Crea un endpoint per la comunicazione e restituisce un descrittore di file
 *	che fa riferimento a tale endpoint.
 * 	@param family 	Definisce il dominio di comunicazione (es. AF_INET=IPv4)
 *	@param type 	Definisce il protocollo di comunicazione
 *					(es. SOCK_STREAM = TCP, SOCK_DGRAM = UDP)
 *	@param protocol	Solitamente impostato a 0
 *	@return 		Descrittore del socket appena creato
 */
int Socket(int family, int type, int protocol) {
	int fd;
	if ((fd = socket(family, type, protocol)) < 0) {
		perror("socket");
		exit(1);
	}
	return fd;
}

/**	Avvia una connessione su un socket.
 * 	@param sockfd 	Descrittore del socket da connettere
 *	@param servaddr	Indirizzo al quale connettere il socket
 */
void Connect(int sockfd, struct sockaddr_in *servaddr) {
	if (connect(sockfd,(struct sockaddr *) servaddr, sizeof(*servaddr)) < 0) {
		perror("connect");
		exit(1);
	}
}

/**	Associa un indirizzo ad un socket.
 * 	@param sockfd	Descrittore del socket da assegnare
 *	@param servaddr	Indirizzo da assegnare al socket
 */
void Bind(int sockfd, struct sockaddr_in *servaddr) {
	if (bind(sockfd, (struct sockaddr *) servaddr, sizeof(*servaddr)) < 0) {
		perror("bind");
		exit(1);
	}
}

/**	Mette il socket in modalità di ascolto in attesa di nuove connessioni.
 * 	@param sockfd		Descrittore del socket da mettere in ascolto
 *	@param queue_length	Dimensione della coda che stabilisce
 *						il numero di connessioni che possono essere in attesa
 */
void Listen(int sockfd, int queue_length) {
	if (listen(sockfd, queue_length) < 0) {
		perror("listen");
		exit(1);
	}
}

/** Accetta una connessione su un socket.
 * 	@param sockfd		Descrittore del socket
 *	@param clientaddr	Indirizzo del client (può essere NULL)
 *	@param addr_dim		Dimensione dell'indirizzo (può essere NULL)
 *	@return				Descrittore del socket associato alla connessione
 */
int Accept(int sockfd, struct sockaddr_in *clientaddr, socklen_t *addr_dim) {
	int connfd;
	if ((connfd = accept(sockfd, (struct sockaddr *) clientaddr, addr_dim)) < 0){
		perror("accept");
		exit(1);
	}
	return connfd;
}

/**	Converte la stringa passata come secondo argomento
 *	in un indirizzo di rete scritto in network order e
 *	lo memorizza nella locazione di memoria puntata dal terzo argomento.
 *	@param af	Tipo di indirizzo (AF_INET = IPv4, AF_INET6 = IPv6)
 *	@param src	Stringa in ingresso contenente l'indirizzo IP
 *	@param dst	Buffer dove viene impostato un puntatore che contiene l'indirizzo IP
 */
void Inet_pton(int af, const char *src, void *dst) {
	if (inet_pton(af, src, dst) <= 0) {
		fprintf(stderr,"inet_pton: %s non è valido\n", src);
		exit(1);
	}
}

/** Recupera le informazioni sull'host corrispondenti a un indirizzo o un nome simbolico.
 * 	@param name	Indirizzo o nome simbolico
 *	@return		Struttura contenente varie informazioni sull'host passato come parametro
 *				(NULL in caso di errore).
 */
struct hostent *GetHostByName(const char *name) {
	struct hostent *host = gethostbyname(name);
	if (host == NULL){
		herror("gethostbyname");
		exit(1);
	}
	return host;
}

/** Recupera le informazioni sull'host corrispondenti a un indirizzo o un nome simbolico.
 * 	@param addr	Indirizzo IP
 *	@param type	Definisce il tipo di indirizzo IP (AF_INET = IPv4, AF_INET6 = IPv6)
 *	@return		Struttura contenente varie informazioni sull'host passato come parametro
 *				(NULL in caso di errore).
 */
struct hostent *GetHostByAddr (const char *addr, int type) {
	struct in_addr addr_net;
	struct hostent *host = gethostbyaddr((const char *) &addr_net, sizeof(addr_net), type);

	inet_aton(addr, &addr_net);

	if (host == NULL) {
		herror("gethostbyaddr");
		exit(1);
	}
	return host;
}

/**	Recupera le opzioni impostate su un socket.
 *	@param sockfd	Descrittore del socket
 *	@param level	Livello del protocollo (SOL_SOCKET = per recuperare le opzioni a livello socket)
 *	@param optname	Nome dell'opzione specificata (es. SO_REUSEADDR riutilizza indirizzi locali)
 *	@param optval	Puntatore al buffer in cui viene restituito il valore per l'opzione richiesta
 *	@param optlen	Puntatore alla dimensione in byte del buffer optval
 */
void GetSockOpt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) {
	if (getsockopt(sockfd, level, optname, optval, optlen) < 0) {
		perror("getsockopt");
		exit(1);
	}
}

/**	Imposta delle opzioni su un socket.
 *	@param sockfd	Descrittore del socket
 *	@param level	Livello del protocollo (SOL_SOCKET = per recuperare le opzioni a livello socket)
 *	@param optname	Nome dell'opzione specificata (es. SO_REUSEADDR riutilizza indirizzi locali)
 *	@param optval	Puntatore al buffer in cui è specificato il valore per l'opzione richiesta
 *	@param optlen	Dimensione in byte del buffer optval
 */
void SetSockOpt(int sockfd, int level, int optname, void *optval, socklen_t optlen) {
	if (setsockopt(sockfd, level, optname, optval, optlen) < 0) {
		perror("setsockopt");
		exit(1);
	}
}

/******************************************************
 **                 I/O MULTIPLEXING                 **
 ******************************************************/

/**	Comunica al kernel di monitorare un insieme di descrittori, ponendo
 *	il processo in waiting e risvegliandolo quando si verifica un evento.
 *	@param maxfdp		Numero di descrittori da monitorare
 *	@param readfds		Insieme di descrittori pronti per la lettura
 *	@param writefds		Insieme di descrittori pronti per la scrittura
 *	@param exceptfds	Insieme di descrittori pronti per la gestione di dati urgenti
 *	@param timeout		Tempo massimo di attesa per un descrittore pronto
 *						(se 0 si adotta il polling; se NULL il tempo di attesa è infinito)
 *	@return	Numero di descrittori disponibili
 */

int Select(int maxfdp, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
	int n;
	if ((n = select(maxfdp, readfds, writefds, exceptfds, timeout)) < 0) {
		perror("select");
		exit(1);
	}
	return n;
}

/******************************************************
 **       FUNZIONI E PROCEDURE DI USO GENERICO       **
 ******************************************************/

/**	Crea un nuovo processo duplicando il processo chiamante.
 *	@return	pid ID del processo (uguale a 0 nel figlio, diverso da 0 nel genitore)
 */
pid_t Fork(void) {
	int pid;
	if ((pid = fork()) < 0) {
		perror ("fork");
		exit(1);
	}
	return pid;
}

/** Legge da un buffer contenuto in un descrittore un numero finito di byte.
 *	Controlla l'operazione della chiamata di sistema read() gestendo gli eventuali
 *	segnali di interrupt e garantendo la completa lettura dal buffer.
 * 	@param fd		Descrittore
 *	@param buf		Buffer dal quale leggere i byte
 *	@param count	Numero di byte da leggere
 *	@return			Numero di byte rimanenti (pari a 0 se l'operazione va a buon fine)
 */
ssize_t Read(int fd, void *buf, size_t count) {
	size_t nleft;	/* numero di byte rimanenti da leggere */
	ssize_t nread;	/* numero di byte letti (utilizzato per gestire gli spiazzamenti per *buf) */
	nleft = count;
	memset(buf, 0, count);
	while (nleft > 0) {
		if ((nread = read(fd, buf, nleft)) < 0) {
			if (errno == EINTR)
				continue;			/* continua se read() è interrotta da chiamata di sistema */
			else {
				perror("read");
				exit(nread);
			}
		}
		else
			if (nread == 0) /* EOF */
				break;
		nleft -= nread; /* imposta il numero di byte rimanenti a quelli da leggere */
		buf += nread;	/* spiazzamento puntatore a buffer */
	}
	buf = 0;
	return nleft;
}

/** Scrive su un buffer contenuto in un descrittore un numero finito di byte.
 *	Controlla l'operazione della chiamata di sistema write() gestendo gli eventuali
 *	segnali di interrupt e garantendo la completa scrittura sul buffer.
 * 	@param fd		Descrittore
 *	@param buf		Buffer sul quale scrivere i byte
 *	@param count	Numero di byte da scrivere
 *	@return			Numero di byte rimanenti (pari a 0 se l'operazione va a buon fine)
 */
ssize_t Write(int fd, const void *buf, size_t count) {
	size_t nleft;		/* numero di byte rimanenti da scrivere */
	ssize_t nwritten;	/* numero di byte scritti (utilizzato per gestire gli spiazzamenti per *buf) */
	nleft = count;
	while (nleft > 0) {
		if ((nwritten = write(fd, buf, nleft)) < 0) {
			if (errno == EINTR)
				continue;			/* continua se write() è interrotta da chiamata di sistema */
			else {
				perror("write");
				exit(nwritten);
			}
		}
		nleft -= nwritten;	/* imposta il numero di byte rimanenti a quelli da scrivere */
		buf +=nwritten;		/* spiazzamento puntatore a buffer */
	}
	return nleft;
}

/**	Apre un file in lettura e/o scrittura.
 *	@param pathname	Nome del file da aprire
 *	@param mode		Permessi
 */
FILE *Fopen(const char *pathname, const char *mode) {
	FILE *file;
	if ((file = fopen(pathname, mode)) == NULL) {
		perror("fopen");
		exit(1);
	}
	return file;
}
