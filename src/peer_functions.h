#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#include <time.h>
#include <sys/timeb.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <openssl/sha.h>
#include <fcntl.h>
#include <limits.h>

#define MAX_SIZE_PUBLISH 500

int recv_tcp(int socketfd, char* buffer, int buff_size);
void send_tcp(int socketfd, char* buffer, int buff_size);
void TCP_server();
void TCP_client(char* peerIP, char* requestedFilePath, char* expectedHash);
void UDP_search();
void UDP_publish();
char* hash_file(char* filePath);
int countChar(char* str, char selection);
char* getNameFromPath(char* filePath);
