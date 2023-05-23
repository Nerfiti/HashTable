#include "hashtable.hpp"

enum OPT_LEVEL
{
    NO_OPTIMIZATION,
    INLINE_ASM,
    ASM
};

char **FillTable(HashTable *table, const char *filename);
void DistributionTest(hash_t (*hashfunc)(elem_t key), FILE *result);
void SpeedTest(OPT_LEVEL level);