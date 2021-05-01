#include "peer_functions.h"

#define SERV_IP "127.0.0.1"
#define TCP_IP "127.0.0.1"
#define SERV_PORT 1025
#define TCP_PORT 2000



int main(int argc, char const *argv[]) {
    int pid_server;
    if ((pid_server = fork()) !=0) {
        
        char choice = 'a';              // On aura : choice = 1 -> recherche par mots clés | choice = 2 -> ajout d'un fichier
        int mauvaiseSelection = 0;

        sleep(1); // pour laisser le temps au serveur de ce lancer et éviter que les prints s'emèlent

        while ((choice != '1') && (choice != '2') && (choice != '3')) {
            if (mauvaiseSelection) 
                printf("[*] Mauvaise selection\n");
            
            printf("\n[*] Que souhaitez vous faire ?\n\n      (1) Recherche sur des mots clés\n      (2) Ajout d'un fichier\n      (3) Quitter\n\n");
            fgets(&choice,2,stdin);
            while (getchar() != '\n') {} // on vide le buffer de stdin

            if (choice=='1')
                UDP_client(1025);


            if (choice=='2')
                UDP_publish(1025);
                //TCP_client(atoi(argv[1]));

            if (choice=='3') {
                kill(pid_server,SIGKILL);    
                exit(13);
            }
            
            if ((choice != '1') && (choice != '2') && (choice != '3')) {
                mauvaiseSelection = 1;
            } else {
                mauvaiseSelection = 0;
            }
            choice = 'a';
        }

    } else {
        //TCP_server(atoi(argv[2]));
    }
}