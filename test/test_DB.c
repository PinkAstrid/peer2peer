#include "../src/server_db.h"

int main(int argc, char const *argv[])
{
    
    db_t* db = createDB("test/content.csv");
    printf("db->entries->[0]->name      : %s        (expected album-klub-des-loosers)\n",db->entries[0]->name);
    printf("db->entries[1]->name        : %s        (expected tati_à_la_piscine)\n",db->entries[1]->name);
    printf("db->entries[1]->keyWords[1] : %s        (expected vacances)\n",db->entries[1]->keyWords[1]);
    printf("db->entries[0]->hash        : %s        (expected se5f4465e4f6s5e4f)\n",db->entries[0]->hash);
    printf("db->entries[1]->keyWordNbr  : %d        (expected 3)\n",db->entries[1]->keyWordNbr);
    printf("size of db                  : %d        (exptected : 3)\n\n\n",db->size);
    
    char* keyWords1[] = { "vacances" , "ennui" };
    char* keyWords2[] = { "surcoté" };
    char* keyWords3[] = { "no_match" , "surcoté" };

    db_t* selection1 = searchByKeyWords(db,keyWords1,2);
    db_t* selection2 = searchByKeyWords(db,keyWords2,1);
    db_t* selection3 = searchByKeyWords(db,keyWords3,2);

    printf("selection1->entries[0]->name : %s     (expected : tati_à_la_piscine)\n",selection1->entries[0]->name);
    printf("selection2->entries[0]->name : %s       (expected : album-klub-des-loosers)\n",selection2->entries[0]->name);
    printf("selection2->entries[1]->name : %s       (expected : frozen_BLUERAYRIP)\n",selection2->entries[1]->name);
    printf("selection2->size             : %d       (expected : 0)\n",selection3->size);

    freeSelection(selection1);
    freeSelection(selection2);
    freeSelection(selection3);
    freeDB(db);
    return 0;
}
