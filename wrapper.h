#ifndef WRAPPER_H
#define WRAPPER_H

#include <sys/socket.h>
#include <stdio.h>			// perror
#include <stdlib.h>			// exit
#include <unistd.h>			// write, read
#include <sys/types.h>		// size_t, ssize_t
#include <arpa/inet.h>		// in_addr struct
#include <errno.h>			// errno
#include <netdb.h>			// gethostby*
#include <string.h>			// memset
#define BUFSIZE 2048

/**	Crea un endpoint per la comunicazione e restituisce un descrittore di file
 *	che fa riferimento a tale endpoint.
 * 	@param family 	Definisce il dominio di comunicazione (es. AF_INET=IPv4)
 *	@param type 	Definisce il protocollo di comunicazione
 *					(es. SOCK_STREAM = TCP, SOCK_DGRAM = UDP)
 *	@param protocol	Solitamente impostato a 0
 *	@return 		Descrittore del socket appena creato
 */
int Socket(int family, int type, int protocol);

/**	Avvia una connessione su un socket.
 * 	@param sockfd 	Descrittore del socket da connettere
 *	@param servaddr	Indirizzo al quale connettere il socket
 */
void Connect(int sockfd, struct sockaddr_in *servaddr);

/**	Associa un indirizzo ad un socket.
 * 	@param sockfd	Descrittore del socket da assegnare
 *	@param servaddr	Indirizzo da assegnare al socket
 */
void Bind(int sockfd, struct sockaddr_in *servaddr);

/**	Mette il socket in modalità di ascolto in attesa di nuove connessioni.
 * 	@param sockfd		Descrittore del socket da mettere in ascolto
 *	@param queue_length	Dimensione della coda che stabilisce 
 *						il numero di connessioni che possono essere in attesa
 */
void Listen(int sockfd, int queue_length);

/** Accetta una connessione su un socket.
 * 	@param sockfd		Descrittore del socket
 *	@param clientaddr	Indirizzo del client (può essere NULL)
 *	@param addr_dim		Dimensione dell'indirizzo (può essere NULL)
 *	@return				Descrittore del socket associato alla connessione
 */
int Accept(int sockfd, struct sockaddr_in *clientaddr, socklen_t *addr_dim);

/**	Converte la stringa passata come secondo argomento
 *	in un indirizzo di rete scritto in network order e
 *	lo memorizza nella locazione di memoria puntata dal terzo argomento.
 *	@param af	Tipo di indirizzo (AF_INET = IPv4, AF_INET6 = IPv6)
 *	@param src	Stringa in ingresso contenente l'indirizzo IP
 *	@param dst	Buffer dove viene impostato un puntatore che contiene l'indirizzo IP
 */
void Inet_pton(int af, const char *src, void *dst);

/** Recupera le informazioni sull'host corrispondenti a un indirizzo o un nome simbolico.
 * 	@param name	Indirizzo o nome simbolico
 *	@return		Struttura contenente varie informazioni sull'host passato come parametro
 *				(NULL in caso di errore).
 */
struct hostent *GetHostByName(const char *name);

/** Recupera le informazioni sull'host corrispondenti a un indirizzo o un nome simbolico.
 * 	@param addr	Indirizzo IP
 *	@param type	Definisce il tipo di indirizzo IP (AF_INET = IPv4, AF_INET6 = IPv6)
 *	@return		Struttura contenente varie informazioni sull'host passato come parametro
 *				(NULL in caso di errore).
 */
struct hostent *GetHostByAddr (const char *addr, int type);

/**	Recupera le opzioni impostate su un socket.
 *	@param sockfd	Descrittore del socket
 *	@param level	Livello del protocollo (SOL_SOCKET = per recuperare le opzioni a livello socket)
 *	@param optname	Nome dell'opzione specificata (es. SO_REUSEADDR riutilizza indirizzi locali)
 *	@param optval	Puntatore al buffer in cui viene restituito il valore per l'opzione richiesta
 *	@param optlen	Puntatore alla dimensione in byte del buffer optval
 */
void GetSockOpt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);

/**	Imposta delle opzioni su un socket.
 *	@param sockfd	Descrittore del socket
 *	@param level	Livello del protocollo (SOL_SOCKET = per recuperare le opzioni a livello socket)
 *	@param optname	Nome dell'opzione specificata (es. SO_REUSEADDR riutilizza indirizzi locali)
 *	@param optval	Puntatore al buffer in cui è specificato il valore per l'opzione richiesta
 *	@param optlen	Dimensione in byte del buffer optval
 */
void SetSockOpt(int sockfd, int level, int optname, void *optval, socklen_t optlen);

/******************************************************
 **       FUNZIONI E PROCEDURE DI USO GENERICO       **
 ******************************************************/
 
/**	Crea un nuovo processo duplicando il processo chiamante.
 *	@return	pid ID del processo (uguale a 0 nel figlio, diverso da 0 nel genitore)
 */
pid_t Fork(void);

/** Legge da un buffer contenuto in un descrittore un numero finito di byte.
 *	Controlla l'operazione della chiamata di sistema read() gestendo gli eventuali
 *	segnali di interrupt e garantendo la completa lettura dal buffer.
 * 	@param fd		Descrittore
 *	@param buf		Buffer dal quale leggere i byte
 *	@param count	Numero di byte da leggere
 *	@return			Numero di byte rimanenti (pari a 0 se l'operazione va a buon fine)
 */
ssize_t Read(int fd, void *buf, size_t count);

/** Scrive su un buffer contenuto in un descrittore un numero finito di byte.
 *	Controlla l'operazione della chiamata di sistema write() gestendo gli eventuali
 *	segnali di interrupt e garantendo la completa scrittura sul buffer.
 * 	@param fd		Descrittore
 *	@param buf		Buffer sul quale scrivere i byte
 *	@param count	Numero di byte da scrivere
 *	@return			Numero di byte rimanenti (pari a 0 se l'operazione va a buon fine)
 */
ssize_t Write(int fd, const void *buf, size_t count);

#endif
