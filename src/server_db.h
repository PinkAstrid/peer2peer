
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#define MAX_DB_SIZE 5
#define IP_LEN 15           // dans le cas le plus long : XXX.XXX.XXX.XXX
#define NAME_LEN 200       // nom du fichier partagé
#define TYPE_LEN 5          // mp3, jpeg ...
#define KEY_WORD_LEN 30     // longueur maximum des mots clé
#define KEY_WORD_NBR 10     // nombre maximum de mots clé
#define HASH_LEN 40         // les hash SHA-1 font 40 charactères
#define COMMA_LEN 3         // il y a 3 points-virgules qui servent de séparateurs
#define TOTAL_ENTRY_LEN (IP_LEN + NAME_LEN + TYPE_LEN + ((KEY_WORD_LEN+1) * KEY_WORD_NBR) - 1 + HASH_LEN + COMMA_LEN)

typedef struct
{
    char* ip;
    char* name;
    char* type;
    char** keyWords;
    int keyWordNbr;
    char* hash;

} db_entry;

typedef struct
{
    db_entry** entries;
    int size;
} db_t;


/// CREATION / SUPRESSION ///
db_t* createDB(char* csvName);
db_entry** createDB_entries(char* csvName, int size);
void fillDB_entries(db_entry** db, char* csvName);
void freeDB(db_t* db);


/// REQUETES ///
db_t* searchByKeyWords(db_t* db, char** keyWords, int keyWordsNbr);
void selectIdsByKeyWord(db_t* db, int* selectedIds, char* keyWord);
void freeSelection(db_t* selection);


/// FONCTIONS BASIQUES ///
int isInKeyWords(db_entry* entry, char* word);
int countChar(char* str, char selection);
int countCSVLines(char* csvName);
