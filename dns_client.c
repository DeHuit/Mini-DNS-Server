/********************************
**      TP SOCKETS TCP/IP      **
**  Hello World UDP (client)   **
********************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> // inet_addr, htons
#include "dns_packet_parsing.h"

#define MSG_MAX_SIZE 80
#define BUF_SIZE 500
#define MSG "Hello World!"

/*=====================
= Programme principal =
======================*/
int main(int argc, char * argv[]) {

    int sock_id, server_port = 53;
    char server_ip[16]; // address as a string
    struct sockaddr_in server_adr;

    char message[MSG_MAX_SIZE];

    if (argc < 4 || argc > 6 || argc == 5) {
        printf("Usage: %s -s @IP_serveur_DNS [-p port] nom_de_domaine\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int i = 1;
    char name[100] = "";
    if (strcmp(argv[i], "-s") == 0) {
        i++;
        strcpy(server_ip, argv[i++]);
    }
    if (strcmp(argv[i], "-p") == 0) {
        i++;
        server_port = atoi(argv[i++]);
    }
    strcpy(name, argv[i]);
    /* Creation d'une socket en mode datagramme  */
    /*-------------------------------------------*/
    sock_id = socket(AF_INET, SOCK_DGRAM, 0);
    if ( sock_id < 0 ) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
    /* Initiatlisation de l'adresse du serveur */
    /*-----------------------------------------*/
    memset(&server_adr, 0, sizeof(server_adr));
    server_adr.sin_family = AF_INET;
    server_adr.sin_port = htons(server_port); // htons: host to net byte order (short int)
    server_adr.sin_addr.s_addr = inet_addr(server_ip);

    /* Envoi du message au serveur */
    /*-----------------------------*/
    int a = build_dns_request(message, name);
    //printf("port %d, adress = %s\n", server_port, server_ip);
    if ((sendto(sock_id, message, a, 0, (struct sockaddr*) &server_adr, sizeof(server_adr))) < 0) {
        perror("sendto error");
        exit(EXIT_FAILURE);
    }

    //printf("--> Message envoyé au serveur.\n");

    /* Réception de la réponse */
    /*-------------------------*/
    char buffer[BUF_SIZE];
    socklen_t server_adr_len;
    if (recvfrom(sock_id, buffer, BUF_SIZE, 0, (struct sockaddr *)&server_adr, &server_adr_len) < 0) {
        perror("recvfrom error");
        exit(EXIT_FAILURE);
    }
    /* --> TRAITEMENT <-- */
    if (no_such_name(buffer) == 3) {
      printf("No such name\n");
    }
    else {
      unsigned char ip_answ[4];
      int ip_a [4];
      ip_from_answer(buffer, ip_answ);
      for (size_t i = 0; i < 4; i++)
        ip_a[i] = ip_answ[i];
      printf("%d.%d.%d.%d\n", ip_a[0], ip_a[1], ip_a[2], ip_a[3]);
    }

    /* Fermeture de la socket */
    /*------------------------*/
    close(sock_id);

    return 0;
}
