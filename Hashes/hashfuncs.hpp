#ifndef HASHFUNCS_HPP
#define HASHFUNCS_HPP

#include "list.hpp"

typedef unsigned int hash_t;

hash_t AlwaysOne(elem_t key);
hash_t FirstChar(elem_t key);
hash_t LenHash  (elem_t key);
hash_t SumChar  (elem_t key);
hash_t RolHash  (elem_t key);
hash_t RorHash  (elem_t key);
hash_t crc32    (elem_t key);

hash_t crc32asm (elem_t key);

extern "C" hash_t crc32allAsm(elem_t key);

#endif