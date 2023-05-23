#include <cassert>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include "tests.hpp"

const int NumberOfTests = 100;
const int Max_words_num = 58000;

char **FillTable(HashTable *table, const char *filename)
{
    assert(table);

    struct stat st = {};
    stat(filename, &st);  

    int fd = open(filename, O_RDONLY);
    char *const buffer = (char *)mmap(0, st.st_size, PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (buffer == MAP_FAILED)
    {
        printf("Error file mapping!\nErrno: %d\n", errno);
        return nullptr;
    }

    char **words = (char **)calloc(Max_words_num, sizeof(char *));
    if (!words)
    {
        printf("Error memory allocation to word list\n");
        return nullptr;
    }

    char *reader = buffer;
    char *writer = reader;
    int word_cnt = 0;
    for (;;)
    {
        while (isalpha(*writer))
            ++writer;

        char endSym = *writer;

        *writer = '\0';

        char *key = (char *)aligned_alloc(64, MAX_WORD_LEN + 1);
        assert(key);
        memset (key, 0, MAX_WORD_LEN + 1);
        strncpy(key, reader, MAX_WORD_LEN);
        
        words[word_cnt] = key;
        
        TableAdd(table, key);
        
        if (endSym == '\0')
            break;

        reader = writer + 1;
        while(!isalpha(*reader) && *reader != '\0')
            ++reader;

        writer = reader;

        ++word_cnt;
        if (word_cnt >= Max_words_num)
            break;
    }

    munmap(buffer, st.st_size);
    close(fd);   

    assert(word_cnt == Max_words_num);

    return words;
}

void DistributionTest(hash_t (*hashfunc)(elem_t key), FILE *result)
{
    HashTable *table = TableCtor(hashfunc);
    char **words = FillTable(table, "dict.txt");

    size_t max_size = 0;
    size_t min_size = Max_words_num + 1;

    for (int i = 0; i < table->size; ++i)
    {
        if (table->arr[i].size > max_size)
            max_size = table->arr[i].size;

        if (table->arr[i].size < min_size)
            min_size = table->arr[i].size;
        
        fprintf(result, "%d\n", table->arr[i].size);
    }

    printf("Max = %lu\nMin = %lu\n", max_size, min_size);

    free(words);
}

void SpeedTest(OPT_LEVEL level)
{
    
    HashTable *table = nullptr;
    if (level == NO_OPTIMIZATION)
        table = TableCtor(crc32);
    else if (level == INLINE_ASM)
        table = TableCtor(crc32asm);
    else if (level == ASM)
        table = TableCtor(crc32allAsm);
    
    char **words = FillTable(table, "dict.txt");
    for (int j = 0; j < NumberOfTests; ++j)
    {
        for (int i = 0; i < Max_words_num; ++i)
        {
            TableFind(table, words[i]);
        }
    }

    free(words);
    TableDtor(table);
}
