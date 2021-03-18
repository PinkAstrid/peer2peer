#include "central_server.h"

#define SERV_IP "127.0.0.1"
#define SERV_PORT 1025

int haveHeader(char* header, char* str) {
    int l = strlen(header);
    for (int i = 0; i < l; i++) {
        if (header[i] != str[i]) {
            return 0;
        }
    }
    return 1;
}

int main()
{   
    db_t* db = createDB("test/content.csv");    // On charge la base de donnée du serveur
    char* keyWords[1]; // vecteur temporaire pour essayer de récupérer des infos depuis le client

    int sockfd;
    char recvbuf[1500];
    char sendbuf[1500];
    int recvLen;
    struct sockaddr_in  serv_addr;
    socklen_t serv_addr_len=sizeof(serv_addr);

    char searchReqHeader[] = "SEARCH : ";
    char publishReqHeader[] = "PUBLISH : ";


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

            if (haveHeader(searchReqHeader,recvbuf)) {  // cas ou on recoit une requête de type SEARCH

                keyWords[0] = recvbuf + strlen(searchReqHeader);

                printf("On lance une recherche sur le mot clé : %s\n\n\n",keyWords[0]);

                db_t* selection = searchByKeyWords(db,keyWords,1);


                if (selection->size > 0) {
                    for (int i = 0; i < selection->size; i++) {
                        strcat(sendbuf,selection->entries[i]->name);
                        strcat(sendbuf,"\n");
                    } 
                } else {
                    strcpy(sendbuf,"Aucune entré de la base de donnée correspondant à cette entrée\n");
                }


                sendto(sockfd, sendbuf, strlen(sendbuf),0, (struct sockaddr *)&serv_addr, serv_addr_len);
                freeSelection(selection);
            }

            if (haveHeader(publishReqHeader,recvbuf)) {
                printf("On ajoute une entrée à la BDD\n");

                printf("TODO : el famoso ajout, comme marqué au dessus, merveilleux non ?\n\n\n");

                strcpy(sendbuf,"PUBLISH-ACK");
                sendto(sockfd, sendbuf, strlen(sendbuf),0, (struct sockaddr *)&serv_addr, serv_addr_len);
            }

            bzero(recvbuf,sizeof(recvbuf));  // on nettoie les buffers
            bzero(sendbuf,sizeof(recvbuf));
        }
    }
}
