#include "peer.h"

#define SERV_IP "127.0.0.1"
#define SERV_PORT 1025

int main()
{
    int serverSocket, recvSize, ttl;
    struct sockaddr_in serv_addr;
    char sendbuf[1500];
    char recvbuf[1500];
    int serv_addr_len = sizeof(serv_addr);

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERV_IP);
    serv_addr.sin_port = htons(SERV_PORT);

    if ( (serverSocket = socket(PF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("Erreur : Probèle lors du lancement de la socket serveur");
        exit(1);
    }

    ttl = 1;
    if (setsockopt(serverSocket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
        perror("Erreur : Problème lors de la mise en place du ttl");
        exit(2);
    }

    bzero(sendbuf,sizeof(sendbuf)); // IMPORTANT : on netttoie les deux buffers avant de les utiliser
    bzero(recvbuf,sizeof(recvbuf));

    stpcpy(sendbuf,"PUBLISH : ici le contenu de la requête"); // ici on met en place le contenu de la requête

    if (sendto(serverSocket, sendbuf, strlen(sendbuf), 0, (struct sockaddr*) &serv_addr, serv_addr_len) != strlen(sendbuf) ) {
        perror("Erreur : Problème lors de l'envoie des données");
        exit(3);
    }

    if ( (recvSize = recvfrom(serverSocket, recvbuf, sizeof(recvbuf)-1, 0, (struct sockaddr*) &serv_addr, &serv_addr_len)) < 0) {
        perror("Erreur : Problème lors de la réception des données du serveur");
        exit(4);
    }

    recvbuf[recvSize] = '\0';
    printf("Message reçu du serveur : %s\n", recvbuf);
    close(serverSocket);

    return 0;
}
