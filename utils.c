#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h> // inet_addr, htons
/* *** PLACER VOS FONCTIONS "UTILITAIRES" ICI *** */

/* Exemple : */

void print_ip_v4(unsigned char ip_addr[4]) {

    printf("%d.%d.%d.%d\n", ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
}

void get_ip4_from_char(unsigned char * ip4, char * ip4str) {
    int ip_int[4];
    sscanf(ip4str, "%d.%d.%d.%d",
           &ip_int[0], &ip_int[1], &ip_int[2], &ip_int[3]);
    for(int i = 0; i < 4; i++)
        ip4[i] = (unsigned char) ip_int[i];
}

void getAuth(char * auth, char * qname) {
    for (size_t i = 0; qname[i] != '.' && qname[i] != '\0'; i++){
        auth[i] = qname[i];
        auth[i+1] = '\0';
    }
}

void getRoot(char * root, char * qname) {
  char * lastPart = strrchr(qname, '.');
  lastPart++;
  strcpy(root, lastPart);
  printf("*****************\n");
  printf("%s\n", lastPart);
  printf("*****************\n");
}

void getTLD(char * tld, char * qname) {
  char delim[2];
  strcpy(delim, ".");
  char * tldPart = strstr(qname, delim);
  tldPart++;
  strcpy(tld, tldPart);
}

int create_socket(int mod, struct sockaddr_in * server_adr, int port){
    int sock = socket(AF_INET, mod, 0);
    if ( sock < 0 ) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    /* Initialisation adresse locale du serveur  */
    memset(server_adr, 0, sizeof(struct sockaddr_in));
    server_adr->sin_family = AF_INET;
    server_adr->sin_port = htons(port); // htons: host to net byte order (short int)
    server_adr->sin_addr.s_addr = htonl(INADDR_ANY); // wildcard address

    if (bind(sock, (struct sockaddr *) server_adr, sizeof(struct sockaddr_in)) < 0) {
        perror("bind error");
        exit(EXIT_FAILURE);
    }
    return sock;
}

void init_new_connection(struct sockaddr_in * server_adr, char * ip, int port) {
    memset(server_adr, 0, sizeof(struct sockaddr_in));
    server_adr->sin_family = AF_INET;
    server_adr->sin_port = htons(port); // htons: host to net byte order (short int)
    server_adr->sin_addr.s_addr = inet_addr(ip);
}