#include "central_server.h"

#define SERV_IP "127.0.0.1"
#define SERV_PORT 1025

int main()
{
    int sockfd;
    char recvbuf[1500];
    char sendbuf[1500];
    int recvLen;
    struct sockaddr_in  serv_addr;
    socklen_t serv_addr_len=sizeof(serv_addr);

    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) <0) { // On lance la socket, SOCK_DGRAM indique qu'on travail en  UDP
        perror ("erreur socket");
        exit (1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERV_IP);
    serv_addr.sin_port = htons(SERV_PORT);

    if (bind(sockfd, (struct sockaddr*) &serv_addr, serv_addr_len) < 0) {
        perror("Erreur : Problème lors du bind de la socket du serveur");
        exit(2);
    }

    bzero(recvbuf, sizeof(recvbuf));    // IMPORTANT : on nettoie les deux buffers avant de les utiliser
    bzero(sendbuf, sizeof(sendbuf));

    for (;;) {
        if ( recvfrom(sockfd, recvbuf, sizeof(recvbuf)-1,0, (struct sockaddr *)&serv_addr, &serv_addr_len) < 0 )  {
                printf ("erreur sendto");
                exit (1);
        } else {
            printf("Données reçues : %s\n",recvbuf);
            
            strcpy(sendbuf,"ACK-");     // on renvoie le message reçu précédé de la chaine "ACK-"
            strcat(sendbuf,recvbuf);
            sendto(sockfd, sendbuf, strlen(sendbuf),0, (struct sockaddr *)&serv_addr, serv_addr_len);
        }
    }
}
