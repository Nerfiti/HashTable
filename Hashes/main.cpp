#include <cstdio>
#include <ctime>

#include "tests.hpp"

int main()
{
    clock_t start = clock();
    SpeedTest(INLINE_ASM);
    clock_t finish = clock();
    
    printf("%ld\n", (1000*(finish - start) / CLOCKS_PER_SEC));
}