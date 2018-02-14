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
#define ROOT_PORT 8888

/*==============
= MAIN PROGRAM =
===============*/

int main(int argc, char * argv[])
{
    int sock_local;
    int sock_server;
    struct sockaddr_in server_adr_this;
    struct sockaddr_in server_adr;
    struct sockaddr_in client_adr;
    socklen_t client_adr_len = sizeof(client_adr);
    int port = SERVER_PORT;
    char client_request[BUF_SIZE];
    char server_request[BUF_SIZE];
    char server_answer[BUF_SIZE];
    char client_answer[BUF_SIZE];
    char root_ip[20] = "127.0.0.1";

    /* Traitement des arguments */
    if (argc != 2) {
        fprintf(stderr, "Usage : %s port\n", argv[0]);
        exit(ERR_WRONG_ARG);
    }

    port = atoi(argv[1]);

    /* Creation d'une socket en mode datagramme  */
    /*-------------------------------------------*/
    sock_local = create_socket(SOCK_DGRAM, &server_adr_this, port);

    /* Réception des messages (attente active)     */
    /* 3 steps:                                    */
    /* 1) Recive client request                    */
    /* 2) Set up root server coordinates & connect */
    /* 3) while(nbanswerinpacket == 0)             */
    /*      send request                           */
    /*      process answer                         */
    /*      terminate old connection               */
    /*      init new connection                    */
    /* 4) answer to client                         */
    /*---------------------------------------------*/

    while (1) {
        printf("\nServeur en attente...\n");
        /* Step 1: clients's request */
        int cl_req_size;
        if ((cl_req_size = (int) recvfrom(sock_local, client_request, BUF_SIZE, 0,
                                 (struct sockaddr *)&client_adr, &client_adr_len)) < 0) {
            perror("recvfrom error");
            exit(EXIT_FAILURE);
        }
        printf("Message recived\n");
        char qname[100]; int nameRecived = -1; int success = 1;
        unsigned char ip[4];
        char server_ip[20];

        nameRecived = qname_from_question(client_request, cl_req_size, qname);
        /* We have to be sure that qname was revied */
        if (nameRecived != -1) {
            /* Step 2: root server */
            sock_server = socket(AF_INET, SOCK_STREAM, 0); //connecting to server in mod STREAM
            init_new_connection(&server_adr, root_ip, ROOT_PORT);
            if(connect(sock_server, (struct sockaddr *) &server_adr, sizeof(server_adr)) < 0) {
                perror("connect error");
                exit(EXIT_FAILURE);
            }

            /* Step 3: cycle */
            unsigned char ip[4];
            success = 1;
            int i = 0;
            do {
                /*Asking server*/
                int request_size = build_dns_request(server_request, qname);
                if (send(sock_server, server_request, (unsigned int) request_size, 0) < 0) {
                    perror("send error");
                    exit(EXIT_FAILURE);
                }
                if (recv(sock_server, server_answer, BUF_SIZE, 0) < 0) {
                    perror("recv error");
                    exit(EXIT_FAILURE);
                }
                /* Processing answer */
                if (no_such_name(server_answer) == 3) {
                    /*If name not found - sending error*/
                    success = -1;
                    break;
                } else {
                    if (i == 2)
                        ip_from_answer(server_answer, ip);
                    else
                        ns_ip_from_add_section(server_answer, ip);
                    sprintf(server_ip, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
                    printf("Look at %s\n", server_ip);
                }
                /* Terminate old connection */
                close(sock_server);

                /* Init new connection  */
                sock_server = socket(AF_INET, SOCK_STREAM, 0);
                init_new_connection(&server_adr, server_ip, ip[0] * ip[3]);

                if(i != 2 && (connect(sock_server, (struct sockaddr *) &server_adr, sizeof(server_adr))) < 0) {
                    perror("connect error");
                    exit(EXIT_FAILURE);
                }
                i++;
            } while (count_answer(server_answer) == 0);
        }
        close(sock_server);
        /* Answer to client */
        if (nameRecived == -1 || success < 0) {
            build_dns_reply_name_error(client_request);
            printf("No such name\n\n");
        }
        else {
            printf("%s at %s\n\n", qname, server_ip);
            build_dns_answer(client_request, ip);
        }

        if (sendto(sock_local, client_request, sizeof(client_answer), 0,
                   (struct sockaddr *)&client_adr, client_adr_len) < 0) {
            perror("sendto error");
            exit(EXIT_FAILURE);
        }


    } /* while */

    close(sock_local);

    return 0;
}
