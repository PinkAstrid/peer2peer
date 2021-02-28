#include "../src/server_db.h"

int main(int argc, char const *argv[])
{
    db_entry** db = createDB("test/content.csv");
    printf("db[0]->name : %s        (expected album-klub-des-loosers)\n",db[0]->name);
    printf("db[1]->name : %s        (expected tati_à_la_piscine)\n",db[1]->name);
    printf("db[1]->keyWords[1] : %s     (expected vacances)\n",db[1]->keyWords[1]);
    printf("db[0]->hash : %s        (expected se5f4465e4f6s5e4f)\n",db[0]->hash);
    printf("db[1]->keyWordNbr : %d        (expected 3)\n",db[1]->keyWordNbr);
    freeDB(db);
    return 0;
}
