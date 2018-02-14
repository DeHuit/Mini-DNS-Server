#ifndef __UTILS_H__
#define __UTILS_H__

/* *** PLACER VOS FONCTIONS "UTILITAIRES" ICI *** */

/* Exemple : */

void print_ip_v4(unsigned char ip_addr[4]);

void get_ip4_from_char(unsigned char * ip4, char * ip4str);

void headAndTail(char * head, char * tail, char * qname);

void getAuth(char * auth, char * qname);

void getRoot(char * root, char * qname);

void getTLD(char * tld, char * qname);

int create_socket(int mod, struct sockaddr_in * server_adr, int port);

void init_new_connection(struct sockaddr_in * server_adr, char * ip, int port);
#endif
