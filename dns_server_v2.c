/*********************************
**      TP SOCKETS TCP/IP       **
**  Hello World UDP (serveur)   **
*********************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> // inet_addr, htons
#include "dns_packet_parsing.h"
#include "dns_server_rr_files.h"
#include "utils.h"

#define BUF_SIZE 500
#define SERVER_PORT 5555
#define ERR_WRONG_ARG 99
#define ROOT 1
#define TLD 2
#define AUTHORITATIVE 3

/*==============
= MAIN PROGRAM =
===============*/

int main(int argc, char * argv[])
{
    int sock_server;
    int sock_this;
    int sock_client;
    struct sockaddr_in server_adr;
    struct sockaddr_in client_adr;
    socklen_t client_adr_len = sizeof(client_adr);
    int port = SERVER_PORT;
    char buffer[BUF_SIZE];

    if (argc != 3) {
      fprintf(stderr, "Usage : %s chmeinverscatalogue port\n", argv[0]);
      exit(ERR_WRONG_ARG);
    }

    char database[100];
    strcpy(database, argv[1]);
    port = atoi(argv[2]);

    /* On determin le domain et type de serveur*/
    char domain[50];
    int type = read_server_type_from_conf(database, domain);
    printf("Domain de serveur : %s, type = %s\n", domain, (type == ROOT)?"ROOT":(type == TLD)?"TLD":"AUTHORITATIVE");

    /* Creation d'une socket en mode datagramme  */
    /*-------------------------------------------*/
    sock_this = create_socket(SOCK_STREAM, &server_adr, port);

    if(listen(sock_this, 5) == -1) {
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    /* Réception des messages (attente active)  */
    /*------------------------------------------*/

    while (1) {
        printf("\nServeur en attente...\n");

        sock_client = accept(sock_this, (struct sockaddr *) &client_adr, &client_adr_len);

        int buff_size;
        //client_adr_len = sizeof(client_adr);
        if ((buff_size = recv(sock_client, buffer, BUF_SIZE, 0)) < 0) {
            perror("recv error");
            exit(EXIT_FAILURE);
        }

        char qname[100], packet[BUF_SIZE], name[100], adresse[100], additionnal[100];
        int nameRecived = -1, dataRecived = 0;
        memcpy(packet, buffer, buff_size);
        nameRecived = qname_from_question(buffer, buff_size, qname);
        //printf("*********QNAME : %s\n", qname);
        /* Name is extracted */
        if (nameRecived != -1) {
          switch (type) {
            case ROOT: getRoot(name, qname); break;
            case TLD : getTLD(name, qname); break;
            case AUTHORITATIVE : getAuth(name, qname); break;
          }
          printf("NAME = %s\n", name);
          if (type != AUTHORITATIVE) {
            dataRecived = get_rr_data_from_name(database, name, additionnal);
            get_rr_data_from_name(database, additionnal, adresse);
          }
          else
            dataRecived = get_rr_data_from_name(database, name, adresse);
        }
        /* Name not found */
        if (nameRecived == -1 || dataRecived == 0) {
          build_dns_reply_name_error(packet);
        }
        else { /* Name foud. Poursuit */
          unsigned char ip4[4];
          get_ip4_from_char(ip4, adresse);
          if (type != AUTHORITATIVE)
            build_dns_additional(packet, name, additionnal,  ip4);
          else
            build_dns_answer(packet, ip4);
        }

        if (send(sock_client, packet, sizeof(packet), 0) < 0) {
            perror("sendto error");
            exit(EXIT_FAILURE);
        }

        close(sock_client);
    } /* while */
    
    close(sock_server);

    return 0;
}
