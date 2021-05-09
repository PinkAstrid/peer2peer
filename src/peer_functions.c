#include "peer_functions.h"
#include "server_db.h"


#define SERV_IP "192.168.1.16"
#define UDP_PORT 1500
#define TCP_PORT 2000

#define NBR_MAX_CLIENT 5
#define MAX_SIZE_TCP_MESSAGE 256

#define SOCK_TIMEOUT 3 //temps en seconde avant timeout (envoi et reception)


///////////////////////////////////////// FONCTIONS D'ECHANGES

void TCP_server_recv(int socket_client) {
    char recvbuf[500];

    recv_tcp(socket_client, recvbuf, 500);
    printf("[*] TCP - server : Message reçu : {%s}\n",recvbuf);
    //send_tcp(socket_client, buffer, strlen(buffer));

    int requestedFile = open(recvbuf, O_RDONLY);
    if (requestedFile <= 0) {
        printf("[*] TCP - server : Fichier %s inexistant\n", recvbuf);
        return;
    } else {
        char sendbuf[MAX_SIZE_TCP_MESSAGE];
        int n;
        while ((n = read(requestedFile, sendbuf, MAX_SIZE_TCP_MESSAGE - 1)) > 0)
        {
            sendbuf[n] = '\0';
            send_tcp(socket_client, sendbuf, n);
        }
        printf("[*] TCP - server : Fichier %s envoyé\n", recvbuf);
    }
}


void send_tcp(int socketfd, char* buffer, int buff_size) {
    if (send(socketfd, buffer, buff_size, 0) < 0)
        printf("[*] TCP : Erreur lors du send\n");
}

void UDP_send(int socket, struct sockaddr_in address, char* buffer, int size) {
    if (sendto(socket, buffer, size, 0, (const struct sockaddr*) &address, sizeof(address)) == -1) {
        printf("[*] UDP : Erreur lors de l'envoi\n");
    }
}


int recv_tcp(int socket, char* buffer, int buff_size) {
    int res = recv(socket, buffer, buff_size, 0);
    if (res > 0)
    {
        buffer[res] = '\0';
    }
    else if (res == 0)
    {
        buffer[0] = '\0';
        printf("[*] TCP : Message de fin de connexion reçu\n");
    }
    else
        printf("[*] TCP : Erreur lors du recv\n");

    return res;
}

///////////////////////////////////////// FONCTIONS REQUETES

void UDP_search() {
    int socket_server, recvSize, ttl;
    struct sockaddr_in serv_addr;
    char sendbuf[1500];
    char recvbuf[1500];
    char userEntry[1400];
    int serv_addr_len = sizeof(serv_addr);
    struct hostent *hp;

    char searchReqHeader[] = "SEARCH ";

    if ((socket_server = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("[*] UDP - client : Erreur lors du lancement du socket\n");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERV_IP);
    serv_addr.sin_port = htons(UDP_PORT);

    printf("Entrez le mot clé :\n");
    fgets(userEntry,1400,stdin);
    userEntry[strlen(userEntry)-1] = '\0';

    strcpy(sendbuf, searchReqHeader);
    strcat(sendbuf,userEntry);


    if (sendto(socket_server, sendbuf, strlen(sendbuf), 0, (struct sockaddr*) &serv_addr, serv_addr_len) != strlen(sendbuf) ) {
        printf("[*] UDP - client : Erreur lors de l'envoi\n");
    }

    if ((recvSize = recvfrom(socket_server, recvbuf, sizeof(recvbuf)-1, 0, (struct sockaddr*) &serv_addr, &serv_addr_len)) < 0) {
        printf("[*] UDP - client : Erreur lors de la reception\n");
    }
    
    recvbuf[recvSize] = '\0';
    //printf("[*] UDP - client : Message reçu : {%s}\n", recvbuf);
    //printf("recvsize : %d\n",recvSize);
    if (strcmp("Aucune entré de la base de donnée correspondant à ces mots-clé\n", recvbuf)) {
        int nbrOfResults = countChar(recvbuf, '\n');
        char* savedRecvbuf = malloc((strlen(recvbuf)+1) * sizeof(char)); // on copy le contenu de recvbuf car il est modifier par l'appel à strtok 
        strcpy(savedRecvbuf, recvbuf);

        char* endLine;
        char* spaces;
        char* result = strtok_r(recvbuf, "\n", &endLine);
        char* isolatedName = strtok_r(result, " ", &spaces);

        for (int i = 0; i < nbrOfResults; i++) { 
            printf("        (%d) %s\n",i+1, getNameFromPath(isolatedName));
            result = strtok_r(NULL, "\n", &endLine);
            isolatedName = strtok_r(result, " ", &spaces);
        }

        printf("\n\nEntrez le numéro associé au fichier que vous voulez télécharger (entrez 0 pour n'en prendre aucun) : ");
        char selectedFile[4];
        fgets(selectedFile, 4, stdin);

        while (atoi(selectedFile) > nbrOfResults) {
            printf("\n\nEntrée invalide");
            printf("\n\nEntrez le numéro associé au fichier que vous voulez télécharger (entrez 0 pour n'en prendre aucun) : ");
            fgets(selectedFile,2,stdin);
        }

        if (atoi(selectedFile) == 0) {
            return;
        } else {
            result = strtok(savedRecvbuf, "\n");             // on récupère le numéro du fichier choisi parmis les résultats
            for (int i = 1; i < atoi(selectedFile); i++) {
                result = strtok(NULL, "\n");
                if (i == 4)
                    return;
            }

            char* requestedFilePath = strtok(result, " ");         // on récupère l'ip du pair associé, car le format du retour du serveur est : nomDuFichier IP hash
            char* peerIP = strtok(NULL, " ");
            char* hash = strtok(NULL, " ");
            TCP_client(peerIP, requestedFilePath, hash);
        }


    } else {
        printf("Aucune entrée de la base de donnée correspondant à ces mots-clé\n");
    }

    close(socket_server);
}

void UDP_publish() {
    
    printf("[*] UDP - client : Entrez le chemin du fichier à envoyer : ");
    char* filePath = malloc(200*sizeof(char));
    char* absoluteFilePath = malloc(200*sizeof(char));
    fgets(filePath,200, stdin);
    filePath[strlen(filePath) - 1] = 0;

    realpath(filePath,absoluteFilePath); // on récupère le chemin absolu

    //printf("strlen filepath : %d\n",strlen(filePath));
    //printf("file path : {%s}\n",filePath);
    //printf("absolute filepath : {%s}\n",absoluteFilePath);

    FILE* sentFile;
    if(!(sentFile = fopen(filePath, "r"))){
        perror("[*] UDP - client : Le fichier n'existe pas");
        return;
    }
    fclose(sentFile);

    char* extension = strrchr(filePath, '.') + 1;
    
    //Récupération des mots-clef
    printf("[*] UDP - client : Entrez les mots-clef correspondant au fichier en les séparant par des slashs sans espaces :\n");
    char* keywords = malloc(1500*sizeof(char));
    fgets(keywords, 1500, stdin); 
    keywords[strlen(keywords) - 1] = 0;

    char* hash = hash_file(filePath);
    if (strlen(hash) != 40) {
        printf("[*] UDP - client : Problème lors de la génération du hash\n");
        return;
    }

    char* bufferEnvoi = malloc(MAX_SIZE_PUBLISH * sizeof(char));
    sprintf(bufferEnvoi, "PUBLISH %s %s %s %s", absoluteFilePath, extension, keywords, hash);

    int socket_server, recvSize, ttl;
    struct sockaddr_in serv_addr;
    char sendbuf[1500];
    char recvbuf[1500];
    char userEntry[1400];
    int serv_addr_len = sizeof(serv_addr);
    struct hostent *hp;

    if ((socket_server = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("[*] UDP - client : Erreur lors du lancement du socket\n");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERV_IP);
    serv_addr.sin_port = htons(UDP_PORT);
    

    UDP_send(socket_server, serv_addr, bufferEnvoi, strlen(bufferEnvoi));
    
    free(filePath); free(keywords); free(bufferEnvoi);
}

///////////////////////////////////////// FONCTIONS FINALES

void TCP_server() {
    printf("[*] TCP - server : Lancement\n");

    int socket_ecoute;
    struct sockaddr_in server_addr, client_addr;

    if ( (socket_ecoute = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("[*] TCP - server : Erreur lors de la création du socket\n");
    } else {
        printf("[*] TCP - server : Socket d'écoute crée\n");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(TCP_PORT);

    struct timeval timer;       // on met en place un timeout pour éviter que le serveur soit bloqué si le client ne répond plus
    timer.tv_sec = SOCK_TIMEOUT;
    timer.tv_usec = 0;
    setsockopt(socket_ecoute, SOL_SOCKET, SO_SNDTIMEO, (const char*) &timer, sizeof(timer));
    setsockopt(socket_ecoute, SOL_SOCKET, SO_RCVTIMEO, (const char*) &timer, sizeof(timer));


    if (bind(socket_ecoute, (struct sockaddr*) &server_addr, sizeof(server_addr)) != 0) {
        printf("[*] TCP - server : Erreur lors du bind\n");
    } else {
        printf("[*] TCP - server : Bind reussi\n");
    }


    if (listen(socket_ecoute,SOMAXCONN) < 0) {
        printf("[*] TCP - server : Erreur lors du listen\n");
    } else {
        printf("[*] TCP - server : Listen reussi\n");
    }

    int tab_client[NBR_MAX_CLIENT];
    fd_set rset, pset;
    int maxfdp1 = socket_ecoute + 1;
    int nbfd;
    int socket_client;

    for (int i = 0; i < FD_SETSIZE; i++) {
        tab_client[i] = -1;
    }
    FD_ZERO(&pset);
    FD_ZERO(&rset);
    FD_SET(socket_ecoute, &rset);


    while(1) {
        pset = rset;
        nbfd = select(maxfdp1, &pset, NULL, NULL, NULL);

        if (FD_ISSET(socket_ecoute, &pset)) {
            int first_dispo = 0;
            while (first_dispo < FD_SETSIZE && tab_client[first_dispo] >= 0) {
                first_dispo++;
            }

            if (first_dispo < FD_SETSIZE) {
                int client_addr_len = sizeof(client_addr);
                memset(&client_addr, 0, client_addr_len);

                int socket_new_client = accept(socket_ecoute, (struct sockaddr*) &client_addr, &client_addr_len);
                tab_client[first_dispo] = socket_new_client;

                FD_SET(socket_new_client, &rset);
                if (socket_new_client >= maxfdp1) {
                    maxfdp1 = socket_new_client + 1;
                }
                nbfd--;

                printf("[*] TCP - server : Nouveau client ajouté\n");
            }
        }


        int j = 0;
        while (nbfd > 0 && j < FD_SETSIZE) {
            if ((socket_client = tab_client[j]) >= 0 && FD_ISSET(socket_client, &pset)) {
                TCP_server_recv(socket_client);

                close(socket_client);
                tab_client[j] = -1;
                FD_CLR(socket_client, &rset);
                printf("[*] TCP - server : Fin de l'échange avec le client\n");
                nbfd--;
            }
        j++; 
        }
    }
    close(socket_ecoute);

    printf("[*] TCP - server : Fermeture de la socket d'écoute\n");
}

void TCP_client(char* peerIP, char* requestedFilePath, char* expectedHash) {
    int tcp_socketfd;
    struct sockaddr_in peer_address;

  
    //création du socket TCP du client
    tcp_socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socketfd == -1)
    {
        printf("[*] TCP - client : Erreur lors de la création du socket\n"); 
        return;
    }
    else
        printf("[*] TCP - client : Socket créé\n");
    

    //adresse du pair
    memset(&peer_address, 0, sizeof(peer_address));
    peer_address.sin_family = AF_INET;
    peer_address.sin_addr.s_addr = inet_addr(peerIP);
    peer_address.sin_port = htons(TCP_PORT);

    struct timeval timer;
    timer.tv_sec = SOCK_TIMEOUT;
    timer.tv_usec = 0;
    setsockopt(tcp_socketfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timer, sizeof timer);
    setsockopt(tcp_socketfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timer, sizeof timer);


    //connexion au pair
    if (connect (tcp_socketfd, (struct sockaddr *) &peer_address, sizeof(peer_address) ) != 0)
    {
        printf("[*] TCP - client :  Erreur lors de la connexion au pair\n");
        return;
    }
    else
        printf("[*] TCP - client : Connexion au pair réussie\n");


    //envoi du message
    char message[500];
    strcpy(message,requestedFilePath);
    send_tcp(tcp_socketfd, message, strlen(message));

    //preparation de la reception du fichier
    char recv_buffer[500];
    //le mode w+ permet d'ouvrir en écriture et créer le fichier si il n'exite pas. Si le fichier existe, il est écrasé.
    
    char* receivedName = malloc((NAME_LEN + 1 )* sizeof(char));
    strcpy(receivedName, "received/");
    strcat(receivedName, getNameFromPath(requestedFilePath));
    FILE* receivedFile = fopen(receivedName, "w+");

    int len;
    len = recv_tcp(tcp_socketfd, recv_buffer, MAX_SIZE_TCP_MESSAGE);
    fwrite(recv_buffer, len, sizeof(char), receivedFile);

    while (len > 0) {
        len = recv_tcp(tcp_socketfd, recv_buffer, MAX_SIZE_TCP_MESSAGE);
        fwrite(recv_buffer, len, sizeof(char), receivedFile);
    }
    fclose(receivedFile);
    
    char* calculatedhHash = hash_file(receivedName);
    //printf("expected hash : {%s}\n calculated hash : {%s}\n",expectedHash, calculatedhHash);

    if (strcmp(calculatedhHash,expectedHash)) {
        printf("[*] TCP - client : Hash invalide, modification du fichier lors de l'échange\n");
        return;
    } else {
        printf("[*] TCP - client : Hash valide\n");
    }

    printf("[*] TCP - client : Le fichier %s à bien été téléchargé et ce trouve dans le dossier received\n", getNameFromPath(receivedName));

    //printf("[*] TCP - client : Message reçu : %s\n",recv_buffer);
    printf("[*] TCP - client : Fermeture du socket\n");

    close(tcp_socketfd);
}

char* hash_file(char* filePath)
{
    char* result = malloc((SHA_DIGEST_LENGTH + 1) * 2);
    memset(result, 0, (SHA_DIGEST_LENGTH + 1) * 2);
    FILE* file = fopen (filePath, "rb");

    if (file != NULL)
    {
        fseek (file, 0, SEEK_END);
        long data_length = ftell (file);
        fseek (file, 0, SEEK_SET);
        char* buffer = malloc(data_length);
        if (buffer != NULL)
        {
            fread (buffer, 1, data_length, file);

            SHA_CTX shactx;
            unsigned char digest[SHA_DIGEST_LENGTH];
            SHA1_Init(&shactx);
            SHA1_Update(&shactx, buffer, data_length);
            SHA1_Final(digest, &shactx);
            int i;
            for (i = 0; i < SHA_DIGEST_LENGTH; i++) {
                sprintf(&result[i*2], "%02x", digest[i]);
            }
            return result; //success
        }

        fclose (file);
    }

    return ""; //failed
}

int countChar(char* str, char selection) {
    int count = 0;
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == selection) {
            count++;
        }
    }
    return count;
}

char* getNameFromPath(char* filePath) {
    char* name = strrchr(filePath, '/'); // on récupère le nom du fichier seul
    if (name == NULL) {   // cas ou il n'y a pas de slash, donc directement le nom du fichier
        name = filePath;
    } else {
        name += 1;
    }
    return name;
}