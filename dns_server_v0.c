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

/*==============
= MAIN PROGRAM =
===============*/
 
int main(int argc, char * argv[])
{
    int sock_id;
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

    /* Creation d'une socket en mode datagramme  */
    /*-------------------------------------------*/
    sock_id = socket(AF_INET, SOCK_DGRAM, 0);
    if ( sock_id < 0 ) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    /* Association adresse et port à la socket (bind)  */
    /*-------------------------------------------------*/

    /* Initialisation adresse locale du serveur  */
    memset(&server_adr, 0, sizeof(server_adr));
    server_adr.sin_family = AF_INET;
    server_adr.sin_port = htons(port); // htons: host to net byte order (short int)
    server_adr.sin_addr.s_addr = htonl(INADDR_ANY); // wildcard address

    if (bind(sock_id, (struct sockaddr *) &server_adr, sizeof(server_adr)) < 0) {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    /* Réception des messages (attente active)  */
    /*------------------------------------------*/
    
    while (1) {
        printf("\nServeur en attente...\n");

        int buff_size;
        client_adr_len = sizeof(client_adr);
        if ((buff_size = recvfrom(sock_id, buffer, BUF_SIZE, 0, (struct sockaddr *)&client_adr, &client_adr_len)) < 0) {
            perror("recvfrom error");
            exit(EXIT_FAILURE);
        }

        char qname[100], name[100], packet[BUF_SIZE], ip_char[16];
        int nameRecived, dataRecived;
        memcpy(packet, buffer, buff_size);
        nameRecived = qname_from_question(buffer, buff_size, qname);
        if (nameRecived != -1) {
         getAuth(name, qname);
         dataRecived = get_rr_data_from_name(database, name, ip_char);
        }
        if (nameRecived == -1 || dataRecived == 0) {
          build_dns_reply_name_error(packet);
        }
        else {
          unsigned char ip4[4];
          get_ip4_from_char(ip4, ip_char);
          build_dns_answer(packet, ip4);
        }
        
        if (sendto(sock_id, packet, sizeof(packet), 0, (struct sockaddr *)&client_adr, client_adr_len) < 0) {
            perror("sendto error");
            exit(EXIT_FAILURE);
        }
    } /* while */

    close(sock_id);

    return 0;
}
