#include "peer.h"

#define SERV_IP "127.0.0.1"
#define SERV_PORT 1025

int main()
{
    int serverSocket, recvSize, ttl;
    struct sockaddr_in serv_addr;
    char* sendbuf;
    char recvbuf[1500];
    char* userEntry = malloc(1400 * sizeof(char));
    int serv_addr_len = sizeof(serv_addr);

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERV_IP);
    serv_addr.sin_port = htons(SERV_PORT);

    char searchReqHeader[] = "SEARCH : ";

    if ( (serverSocket = socket(PF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("Erreur : Probèle lors du lancement de la socket serveur");
        free(userEntry);
        exit(1);
    }

    ttl = 1;
    if (setsockopt(serverSocket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
        perror("Erreur : Problème lors de la mise en place du ttl");
        free(userEntry);
        exit(2);
    }

    // IMPORTANT : on netttoie les deux buffers avant de les utiliser
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
        fgets(userEntry,1400,stdin); // ici l'utilisateur entre le contenu de la requête
        userEntry[strlen(userEntry)-1] = '\0'; // on retire le charactère '\n' (retour à la ligne)
        sendbuf = malloc(1500*sizeof(char));
        strcpy(sendbuf, searchReqHeader);
        strcat(sendbuf, userEntry);
    }

    if (choice == '2') {
        sendbuf = addFileToServer();
    }

    //while(getchar() != '\n') {}  // on vide le buffer de stdin // inutile pour le moment car on relance peer à chaque fois, mais sera utile plus tard;

    
    if (sendto(serverSocket, sendbuf, strlen(sendbuf), 0, (struct sockaddr*) &serv_addr, serv_addr_len) != strlen(sendbuf) ) {
        perror("Erreur : Problème lors de l'envoie des données");
        free(sendbuf); free(userEntry);
        exit(3);
    }

    if ( (recvSize = recvfrom(serverSocket, recvbuf, sizeof(recvbuf)-1, 0, (struct sockaddr*) &serv_addr, &serv_addr_len)) < 0) {
        perror("Erreur : Problème lors de la réception des données du serveur");
        free(sendbuf); free(userEntry);
        exit(4);
    }

    recvbuf[recvSize] = '\0';
    printf("Message reçu du serveur : %s\n", recvbuf);
    close(serverSocket);

    free(sendbuf); free(userEntry);

    return 0;
}

char* addFileToServer(){
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
        strcpy(keyWords+countKeyWord*50,tinyBuffer); //copie du mot-clef
        tinyBuffer = strtok(NULL, " "); //Mot-clé suivant
        countKeyWord++; //emplacement suivant
    }

    free(tinyBuffer);

    //On a récupérer le fichier s'il existe et les mots-clefs
    //Préparation de l'envoi : flagEnvoi/filePath/mot1/mot2/mot3
    char* bufferEnvoi = malloc(1500*sizeof(char));
    strcpy(bufferEnvoi, "PUBLISH : ");
    strcat(bufferEnvoi,filePath); strcat(bufferEnvoi, "/");
    for (int i=0; i<countKeyWord; i++){
        strcat(bufferEnvoi,keyWords+i*50); strcat(bufferEnvoi, "/");
    }

    //Libération avant sortie
    free(bufferstdin); free(filePath); free(keyWords);

    //Fin du add
    return bufferEnvoi;
}