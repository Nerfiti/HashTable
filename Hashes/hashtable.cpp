#include <cassert>
#include <cstdlib>
#include <cstring>
#include <malloc.h>
#include <immintrin.h>

#include "hashtable.hpp"

//----------------------------------------------------------------------------

HashTable *TableCtor(hash_t (*hashfunc)(const char *key), size_t size)
{
    HashTable *table = (HashTable *)calloc(1, sizeof(HashTable));
    table->hashfunc = hashfunc;
    table->size     = size;

    table->arr = (List *)calloc(table->size, sizeof(List));
    for (int i = 0; i < table->size; ++i)
        listCtor(table->arr + i);

    return table;
}

void TableAdd(HashTable *table, const char *word)
{
    List *list = table->arr + (table->hashfunc(word) % table->size);
    listPushBack(list, word);
}

bool TableFind(HashTable *table, const char *key)
{
    List *list = table->arr + (table->hashfunc(key) % table->size);
    return listSearch(list, key);
}

void TableErase(HashTable *table, const char *key)
{
    List *list = table->arr + (table->hashfunc(key) % table->size);
    int pos = listSearch(list, key);
    listDelete(list, pos);
}

void TableDtor(HashTable *table)
{
    for (int i = 0; i < table->size; ++i)
        listDtor(table->arr + i);

    free(table->arr);
    free(table);
}

//----------------------------------------------------------------------------
