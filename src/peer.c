#include "peer.h"

#define SERV_IP "127.0.0.1"
#define SERV_PORT 1025

int main()
{
    int serverSocket, recvSize, ttl;
    struct sockaddr_in serv_addr;
    char sendbuf[1500];
    char recvbuf[1500];
    char userEntry[1400];
    int serv_addr_len = sizeof(serv_addr);

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERV_IP);
    serv_addr.sin_port = htons(SERV_PORT);

    char searchReqHeader[] = "SEARCH : ";
    char publishReqHeader[] = "PUBLISH : ";

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

    char choice = 'a';              // On aura : choice = 1 -> recherche par mots clés | choice = 2 -> ajout d'un fichier
    int mauvaiseSelection = 0;

    printf("Que souhaitez vous faire ?\n\n      (1) Recherche sur des mots clés\n      (2) Ajout d'un fichier\n\n");

    while ((choice != '1') && (choice != '2')) {
        if (mauvaiseSelection) {
            printf("Mauvaise selection\n\n");
            printf("Que souhaitez vous faire ?\n\n      (1) Recherche sur des mots clés\n      (2) Ajout d'un fichier\n\n");
        }
        fgets(&choice,2,stdin);
        while (getchar() != '\n') {} // on vide le buffer de stdin
        mauvaiseSelection = 1;
    }


    if (choice == '1') {
        printf("Entrez le mot clé :\n");
    }

    if (choice == '2') {
        printf("Entrez le chemin du fichier :\n");
    }

    fgets(userEntry,1400,stdin); // ici l'utilisateur entre le contenu de la requête
    //while(getchar() != '\n') {}  // on vide le buffer de stdin // inutile pour le moment car on relance peer à chaque fois, mais sera utile plus tard;

    userEntry[strlen(userEntry)-1] = '\0'; // on retire le charactère '\n' (retour à la ligne)

    if (choice == '1') {
        strcpy(sendbuf,searchReqHeader);
    }

    if (choice == '2') {
        strcpy(sendbuf,publishReqHeader);
    }

    strcat(sendbuf,userEntry);

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

int addFileToServer(){
    //Préparation des données
    char* bufferstdin = malloc(1500*sizeof(char));
    char* filePath = malloc(1500*sizeof(char));
    char* keyWords = malloc(10*50*sizeof(char)); //tableau de strings (de taille 50 maximum chacun)
    
    //Récupération du fichier
    printf("Veuillez entrer le chemin du fichier à envoyer.\n");
    fgets(filePath,1500, stdin); 
    filePath[strlen(filePath)-1]="\0"; //suppression du charactère \n

    FILE* sentFile;
    if(!(sentFile = fopen(filePath, "r"))){
        perror("Erreur : le fichier que vous voulez envoyer n'existe pas");
    }
        
    //Récupération des mots-clef
    printf("Veuillez entrer les mots-clef correspondant au fichier en les séparant par un espace.");
    fgets(bufferstdin, 1500, stdin); 
    bufferstdin[strlen(bufferstdin)-1]="\0"; //suppression du charactère \n

    int countKeyWord = 0;
    char* tinyBuffer = malloc(50);
    tinyBuffer = strtok(bufferstdin, " ");

    while (countKeyWord<10 && tinyBuffer!=NULL) //Tant qu'on ne dépasse pas le nb max de mot-clef et qu'il en reste à récupérer
    {
        strcpy(keyWords[countKeyWord*50],tinyBuffer); //copie du mot-clef
        tinyBuffer = strtok(NULL, " "); //Mot-clé suivant
        countKeyWord++; //emplacement suivant
    }

    free(tinyBuffer);

    //On a récupérer le fichier s'il existe et les mots-clefs
    //TODO : récup l'adresse IP, envoyer, la récupération depuis le serveur, insertion dans le pgm    


    //Libération avant sortie
    free(bufferstdin); free(filePath); free(keyWords);

    //Fin du add
    return 0;
}