#include "server_db.h"


db_entry** createDB(char* csvName) {
    int i;
    
    db_entry** db = calloc(DB_SIZE,sizeof(db_entry*));

    for (i = 0; i < DB_SIZE; i++) {
        db[i] = calloc(1,sizeof(db_entry));

        db[i]->ip = calloc(IP_LEN,sizeof(char));
        db[i]->name = calloc(NAMEL_LEN,sizeof(char));   // L'array de mots clé est initialisé dans la fonction fillDB car il

        db[i]->type = calloc(TYPE_LEN,sizeof(char));    // faut connaitre leur nombre
        db[i]->hash = calloc(HASH_LEN,sizeof(char));
    }

    fillDB(db,csvName);
    return db;
}

void fillDB(db_entry** db, char* csvName) {
    int i = 0;
    char currentLine[TOTAL_ENTRY_LEN];

    FILE* csvp = fopen(csvName,"r");

    if (csvp == NULL) {
        perror("Erreur : Pointeur null lors de l'ouverture du fichier csv");
        exit(11);
    }

    while(fgets(currentLine, TOTAL_ENTRY_LEN, csvp) != NULL) {
        char* ip = strtok(currentLine,";\n");
        char* name = strtok(NULL,";\n");
        char* type = strtok(NULL,";\n");
        char* keyWords = strtok(NULL,";\n");
        char* hash = strtok(NULL,";\n");

        strcpy(db[i]->ip,ip);
        strcpy(db[i]->name,name);
        strcpy(db[i]->type,type);
        strcpy(db[i]->hash,hash);

        int keyWordNbr = countChar(keyWords,'/') + 1;       // on ajoute 1 au nombre de séparateur pour avoir le nombre de mots clé
        db[i]->keyWordNbr = keyWordNbr;
        db[i]->keyWords = calloc(keyWordNbr,sizeof(char*));

        for (int j = 0; j < keyWordNbr; j++) {
            db[i]->keyWords[j] = calloc(KEY_WORD_LEN,sizeof(char));
        }

        int k = 0;
        char* currentKeyWord = strtok(keyWords,"/");
        while (currentKeyWord != NULL) {
            strcpy(db[i]->keyWords[k],currentKeyWord);
            currentKeyWord = strtok(NULL,"/");
            k++;
        }

        i++;
    }

    fclose(csvp);
}

void freeDB(db_entry** db) {
    int i, j;

    for (i = 0; i < DB_SIZE; i++) {
        free(db[i]->ip);
        free(db[i]->name);
        free(db[i]->type);
        free(db[i]->hash);

        for (j = 0; j < db[i]->keyWordNbr; j++) {
            free(db[i]->keyWords[j]);
        }
        free(db[i]->keyWords);

        free(db[i]);
    }
    free(db);
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