#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <cstdio>

#include "hashfuncs.hpp"
#include "list.hpp"

//----------------------------------------------------------------------------

const unsigned int TableSize = 10601;

//----------------------------------------------------------------------------

struct HashTable
{
    List *arr = nullptr;

    unsigned int size = 0;
    hash_t (*hashfunc)(const char *key) = nullptr;
};

//----------------------------------------------------------------------------

HashTable  *TableCtor   (unsigned int (*hashfunc)(const char *key), size_t size = TableSize);
void        TableAdd    (HashTable *table, const char *key);
bool        TableFind   (HashTable *table, const char *key);
void        TableErase  (HashTable *table, const char *key);
void        TableDtor   (HashTable *table);

//----------------------------------------------------------------------------

#endif