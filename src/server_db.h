
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#define DB_SIZE 2
#define IP_LEN 15           // dans le cas le plus long : XXX.XXX.XXX.XXX
#define TYPE_LEN 5          // mp3, jpeg ...
#define KEY_WORD_LEN 30     // longueur maximum des mots clé
#define KEY_WORD_NBR 10     // nombre maximum de mots clé
#define HASH_LEN 40         // les hash SHA-1 font 40 charactères
#define COMMA_LEN 3         // il y a 3 points-virgules qui servent de séparateurs
#define TOTAL_ENTRY_LEN (IP_LEN + TYPE_LEN + ((KEY_WORD_LEN+1) * KEY_WORD_NBR) - 1 + HASH_LEN + COMMA_LEN)

typedef struct
{
    char* ip;
    char* type;
    char** keyWords;
    int keyWordNbr;
    char* hash;

} db_entry;

db_entry** createDB(char* csvName);
void fillDB(db_entry** db, char* csvName);
void freeDB(db_entry** db);
int countChar(char* str, char selection);
