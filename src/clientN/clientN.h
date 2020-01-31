#include "../../include/wrapper.h"
#include "../../include/pct_n.h"
#include "../../include/lista.h"
#include "../../include/pct_l.h"

void printLogo();
void login(char *usr_login, int sfd);
void signup(int sfd);
void startScreen(int sockfd, char *usr_login);
char menu(char *user);
void insNegozio(int sockfd, char *user);
void cancNegozio(int sockfd, char *user);
void insProdotto(int sockfd, char *user);
void cancProdotto(int sockfd, char *user);
void visualizzaNegozi(int sockfd, char *user);
void visualizzaProdottiNeg(int sockfd, char *user);