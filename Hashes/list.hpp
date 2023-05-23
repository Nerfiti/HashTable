#ifndef LIST_H
#define LIST_H

#include <cstdio>

typedef const char *elem_t;

extern const elem_t STD_POISON;
extern const size_t STD_LIST_SIZE;
extern const size_t MAX_WORD_LEN;

struct list_item
{
    elem_t data = 0;
    size_t next = 0;
    size_t prev = 0;
};

struct List
{
    int size          = -1;
    int capacity      = -1;
    int init_capacity = -1; 
    int free          = -1;
    bool isSorted     = false;

    list_item *buf = nullptr;

    elem_t POISON = STD_POISON;
};

enum Errs
{
    List_OK              =  0,
    WrongListPtr         = -1,
    Non_existent_element = -2,
    DumpError            = -3
};

int listCtor      (List *list_ptr, int capacity = STD_LIST_SIZE, elem_t  POISON = STD_POISON);
int listInsert    (List *list_ptr, elem_t  item, int position);
int listPushBack  (List *list_ptr, elem_t  item);
int listPushFront (List *list_ptr, elem_t  item);
int listDelete    (List *list_ptr, int position, elem_t *item = nullptr);
int listPopBack   (List *list_ptr, elem_t *item = nullptr);
int listPopFront  (List *list_ptr, elem_t *item = nullptr);
int listLinearize (List *list_ptr);
int getPosition   (List *list_ptr, int logical_pos);
int listDtor      (List *list_ptr);

int listSearch    (List *list_ptr, const elem_t key);

#endif //LIST_H