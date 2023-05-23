#include <cassert>
#include <cstring>
#include <malloc.h>
#include <immintrin.h>

#include "list.hpp"

//----------------------------------------------------------------------------
//CONSTANTS
//----------------------------------------------------------------------------

const elem_t STD_POISON    = nullptr;
const size_t STD_LIST_SIZE = 8;
const size_t MAX_WORD_LEN  = 31;

static const int INVALID_ELEMENT_NUM =  0;
static const int FACTOR              =  2;
static const int FREE_INDICATOR      = -1;

//----------------------------------------------------------------------------

static void initFillValues (List *list_ptr);
static void fillPoison     (List *list_ptr);
static void listResizeUP   (List *list_ptr);
static void listResizeDOWN (List *list_ptr);
static bool isFree         (list_item item);

//----------------------------------------------------------------------------

int listCtor(List *list_ptr, int capacity, elem_t POISON)
{
    assert(capacity >= 0);    

    list_ptr->size          = 0;
    list_ptr->capacity      = capacity;
    list_ptr->init_capacity = capacity; 
    list_ptr->free          = 1;

    list_ptr->buf = (list_item *)calloc(capacity + 1, sizeof(list_item));
    
    list_ptr->POISON = POISON;

    initFillValues(list_ptr);

    return List_OK;
}

int listInsert(List *list_ptr, elem_t item, int position)
{   
    if (position != list_ptr->size)
    {
        list_ptr->isSorted = false;
    }

    listResizeUP(list_ptr);

    int pos        = list_ptr->free;
    list_ptr->free = list_ptr->buf[pos].next;

    list_ptr->buf[pos].data = item;
    list_ptr->buf[pos].next = list_ptr->buf[position].next;
    list_ptr->buf[pos].prev = position;

    list_ptr->buf[position].next                = pos;
    list_ptr->buf[list_ptr->buf[pos].next].prev = pos;

    list_ptr->size++;

    return pos;
}

int listPushBack(List *list_ptr, elem_t item)
{
    return listInsert(list_ptr, item, list_ptr->buf[INVALID_ELEMENT_NUM].prev);
}

int listPushFront(List *list_ptr, elem_t item)
{
    return listInsert(list_ptr, item, INVALID_ELEMENT_NUM);
}

int listDelete(List *list_ptr, int position, elem_t *item)
{
    assert(list_ptr);

    if (position == INVALID_ELEMENT_NUM)
    {
        return Non_existent_element;
    }

    int head = list_ptr->buf[INVALID_ELEMENT_NUM].next;
    int tail = list_ptr->buf[INVALID_ELEMENT_NUM].prev;
    if (position != head && position != tail)
    {
        list_ptr->isSorted = false;
    }

    listResizeDOWN(list_ptr);

    if (item != nullptr)
    {
        *item = list_ptr->buf[position].data;
    }
    
    list_ptr->buf[list_ptr->buf[position].prev].next = list_ptr->buf[position].next;
    list_ptr->buf[list_ptr->buf[position].next].prev = list_ptr->buf[position].prev;
    
    list_ptr->buf[position].data = list_ptr->POISON;
    list_ptr->buf[position].next = list_ptr->free;
    list_ptr->buf[position].prev = -1;

    list_ptr->free = position;

    return List_OK;
}

int listPopBack(List *list_ptr, elem_t *item)
{
    return listDelete(list_ptr, list_ptr->buf[INVALID_ELEMENT_NUM].prev, item);
}

int listPopFront(List *list_ptr, elem_t *item)
{
    return listDelete(list_ptr, list_ptr->buf[INVALID_ELEMENT_NUM].next, item);
}

int listLinearize(List *list_ptr)
{
    assert(list_ptr);

    int cur_num = list_ptr->buf[INVALID_ELEMENT_NUM].next;
    elem_t *temp_buff = (elem_t *)calloc(list_ptr->size + 1, sizeof(elem_t));
    for (int i = 1; i <= list_ptr->size; ++i)
    {
        temp_buff[i] = list_ptr->buf[cur_num].data;
        cur_num = list_ptr->buf[cur_num].next;
    }

    for (int i = 1; i <= list_ptr->size; ++i)
    {
        list_item *item = list_ptr->buf + i;
        
        item->data = temp_buff[i];
        item->next = i + 1;
        item->prev = i - 1;
    }
    list_ptr->buf[list_ptr->size].next      = INVALID_ELEMENT_NUM;
    list_ptr->buf[INVALID_ELEMENT_NUM].prev = list_ptr->size;
    list_ptr->buf[INVALID_ELEMENT_NUM].next = 1;

    for (int i = list_ptr->size + 1; i <= list_ptr->capacity; ++i)
    {
        list_item *item = list_ptr->buf + i;

        item->data = list_ptr->POISON;
        item->next = i + 1;
        item->prev = - 1;
    }
    list_ptr->buf[list_ptr->capacity].next      = INVALID_ELEMENT_NUM;
    
    list_ptr->free = list_ptr->size + 1;

    list_ptr->isSorted = true;
    
    return List_OK;
}

int getPosition(List *list_ptr, int logical_pos)
{
    if (list_ptr->isSorted)
    {
        if (logical_pos > list_ptr->size)
        {
            return Non_existent_element;
        }
        return logical_pos;
    }
    
    list_item *item_ptr = list_ptr->buf;
    for (int i = 0; i < logical_pos; ++i)
    {
        item_ptr = list_ptr->buf + item_ptr->next;
    }
    return item_ptr->next;
}

int listDtor(List *list_ptr)
{
    fillPoison(list_ptr);
    list_ptr->free = -1;
    list_ptr->size = -1;
    list_ptr->capacity = -1;
    list_ptr->init_capacity = -1;
    list_ptr->isSorted = false;

    if (list_ptr->buf != nullptr)
    {
        free(list_ptr->buf);
    }
    list_ptr->buf = nullptr;

    return List_OK;
}

//----------------------------------------------------------------------------

static void initFillValues(List *list_ptr)
{
    for (int i = 0; i <= list_ptr->capacity; ++i)
    {
        list_ptr->buf[i].data = list_ptr->POISON;
        list_ptr->buf[i].next = i + 1;
        list_ptr->buf[i].prev = -1;
    }
    list_ptr->buf[INVALID_ELEMENT_NUM].next = INVALID_ELEMENT_NUM;
    list_ptr->buf[INVALID_ELEMENT_NUM].prev = INVALID_ELEMENT_NUM;

    list_ptr->buf[list_ptr->capacity ].next = INVALID_ELEMENT_NUM;
}

static void fillPoison(List *list_ptr)
{
    for (int i = 0; i <= list_ptr->capacity; ++i)
    {
        if (list_ptr->buf[i].data != list_ptr->POISON)
            free((char *)list_ptr->buf[i].data);

        list_ptr->buf[i].data = list_ptr->POISON;
        list_ptr->buf[i].next = 0;
        list_ptr->buf[i].prev = -1;
    }
}

static void listResizeUP(List *list_ptr)
{
    if (list_ptr->size >= list_ptr->capacity)
    {
        list_ptr->buf       = (list_item *)realloc(list_ptr->buf, sizeof(list_item)*(FACTOR*list_ptr->capacity + 1));
        list_ptr->capacity *= FACTOR;
        
        for (int i = list_ptr->size + 1; i <= list_ptr->capacity; ++i)
        {
            list_item *item = list_ptr->buf + i;
            
            item->data = list_ptr->POISON;
            item->next = i+1;
            item->prev = -1;
        }

        list_ptr->buf[list_ptr->capacity].next = INVALID_ELEMENT_NUM;

        list_ptr->free = list_ptr->size + 1;
    }
}

static void listResizeDOWN(List *list_ptr)
{
    if (!list_ptr->isSorted)
        return;

    if (list_ptr->size*FACTOR*FACTOR < list_ptr->capacity)
    {
        list_ptr->capacity /= FACTOR;

        list_ptr->buf = (list_item *)realloc(list_ptr->buf, sizeof(list_item)*(list_ptr->capacity + 1));
    }

    list_ptr->buf[list_ptr->capacity].next = INVALID_ELEMENT_NUM;
}

static bool isFree(list_item item)
{
    return item.prev == FREE_INDICATOR;
}

//----------------------------------------------------------------------------

int listSearch(List *list_ptr, const elem_t key)
{
    __m256i mmKey = _mm256_load_si256((__m256i *) key);

    list_item *buffer = list_ptr->buf;  
    size_t last_index = list_ptr->buf[INVALID_ELEMENT_NUM].next;

    for (int i = 0; i < list_ptr->size; ++i)
    {
        __m256i cur        = _mm256_load_si256((__m256i *) list_ptr->buf[last_index].data);
        __mmask32 cmp_mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(mmKey, cur));
        
        if (!~cmp_mask) 
           return last_index;
           
        last_index = buffer[last_index].next;            
    }
    return INVALID_ELEMENT_NUM; 
}

//----------------------------------------------------------------------------
