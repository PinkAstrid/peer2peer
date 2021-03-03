#include "server_db.h"

/*
        #############################
        #                           #
        #   CREATION / SUPRESSION   #
        #                           #
        #############################
*/

db_t* createDB(char* csvName) {
    int size = countCSVLines(csvName);

    if (size > MAX_DB_SIZE) {
        printf("Erreur : nombre d'entrée du csv ( %d ) supérieur à la taille maximale autorisée ( %d )\n",size,MAX_DB_SIZE);
        exit(12);
    }

    db_t* db = calloc(1,sizeof(db_t));

    db->size = size;
    db->entries = createDB_entries(csvName, size);

    return db;
}


db_entry** createDB_entries(char* csvName, int dbSize) {
    int i;
    
    db_entry** entries = calloc(dbSize,sizeof(db_entry*));

    for (i = 0; i < dbSize; i++) {
        entries[i] = calloc(1,sizeof(db_entry));

        entries[i]->ip = calloc(IP_LEN,sizeof(char));
        entries[i]->name = calloc(NAME_LEN,sizeof(char));   // L'array de mots clé est initialisé dans la fonction fillDB car il

        entries[i]->type = calloc(TYPE_LEN,sizeof(char));    // faut connaitre leur nombre
        entries[i]->hash = calloc(HASH_LEN,sizeof(char));
    }

    fillDB_entries(entries,csvName);
    return entries;
}

void fillDB_entries(db_entry** entries, char* csvName) {
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


        strcpy(entries[i]->ip,ip);
        strcpy(entries[i]->name,name);
        strcpy(entries[i]->type,type);
        strcpy(entries[i]->hash,hash);

        int keyWordNbr = countChar(keyWords,'/') + 1;       // on ajoute 1 au nombre de séparateur pour avoir le nombre de mots clé
        entries[i]->keyWordNbr = keyWordNbr;
        entries[i]->keyWords = calloc(keyWordNbr,sizeof(char*));


        for (int j = 0; j < keyWordNbr; j++) {
            entries[i]->keyWords[j] = calloc(KEY_WORD_LEN,sizeof(char));
        }

        int k = 0;
        char* currentKeyWord = strtok(keyWords,"/");
        while (currentKeyWord != NULL) {
            strcpy(entries[i]->keyWords[k],currentKeyWord);
            currentKeyWord = strtok(NULL,"/");
            k++;
        }

        i++;
    }

    fclose(csvp);
}

void freeDB(db_t* db) {
    int i, j;

    for (i = 0; i < db->size; i++) {
        free(db->entries[i]->ip);
        free(db->entries[i]->name);
        free(db->entries[i]->type);
        free(db->entries[i]->hash);

        for (j = 0; j < db->entries[i]->keyWordNbr; j++) {
            free(db->entries[i]->keyWords[j]);
        }
        free(db->entries[i]->keyWords);

        free(db->entries[i]);
    }
    free(db->entries);
    free(db);
}


/*
        #############################
        #                           #
        #         REQUETES          #
        #                           #
        #############################
*/


db_t* searchByKeyWords(db_t* db, char** keyWords, int keyWordsNbr) {

    int selectedIds[db->size];              // Le vecteur selectedIds indique les id des db_entry de la db qui sont sélectionnés.
                                            // Si selectedIds[i] == 1, c'est que l'entrée i de la db est sélectionnée, si selectedIds[i] == 0 elle ne l'est pas

    for (int i = 0; i < db->size; i++) {    // On commence par considérer que toutes les entrées de la db sont valides
        selectedIds[i] = 1;
    }

    for (int j = 0; j < keyWordsNbr; j++) {
        selectIdsByKeyWord(db,selectedIds,keyWords[j]);   // Pour chaque mot clé, on retire des selectedIds les entrées qui ne correspondent pas
    }

    db_t* selection = calloc(1,sizeof(db_t));
    selection->size = 0;

    for (int k = 0; k < db->size; k++) {
        selection->size += selectedIds[k];  // En sommant toutes les valeurs de selectedIds, on obtient le nombre final d'entrées sélectionnées
    }
    
    selection->entries = calloc(selection->size,sizeof(db_entry*));
    int selectionEntriesCount = 0;

    for (int l = 0; l < db->size; l++) {
        if (selectedIds[l]) {
            selection->entries[selectionEntriesCount] = db->entries[l]; // La selection n'est donc qu'une collection de pointeur vers des entrées de la db
            selectionEntriesCount++;                                    // Il suffira donc de free cet ensemble de pointeur une fois l'utilisation terminée
        }                                                               // et il ne faudra donc pas free les db_entry elles mêmes
    }

    return selection;
}

void selectIdsByKeyWord(db_t* db, int* selectedIds, char* keyWord) {    // met à jour le vecteur de selection selectedIds
    for (int i = 0; i < db->size; i++) {
        if (selectedIds[i]) {
            if(!isInKeyWords(db->entries[i],keyWord)) {
                selectedIds[i] = 0;
            }
        }
    }
}

void freeSelection(db_t* selection) {   // Les selection ne sont qu'une collection de pointeurs vers des entrées de la db, on ne free donc que ces pointeurs
    free(selection->entries);           // et non les db_entry elles mêmes
    free(selection);
}

/*
        #############################
        #                           #
        #    FONCTIONS BASIQUES     #
        #                           #
        #############################
*/

int isInKeyWords(db_entry* entry, char* word) {     // renvoi 1 si le mot est un des mot clé de l'entrée, 0 sinon
    for (int i = 0; i < entry->keyWordNbr; i++) {
        if (strcmp(entry->keyWords[i],word) == 0) {
            return 1;
        }
    }
    return 0;
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

int countCSVLines(char* csvName) {
    int lines = 0;
    char c;

    FILE* f = fopen(csvName,"r");

    if (f == NULL) {
        perror("Erreur : Pointeur null lors de l'ouverture du fichier csv");
        exit(11);
    }

    while(!feof(f)) {
        c = fgetc(f);
        if(c == '\n') {
            lines++;
        }
    }

    fclose(f);
    return lines;
}