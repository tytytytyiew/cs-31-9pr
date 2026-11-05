#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_GLIBC
#define my_malloc malloc
#define my_free free
#define my_realloc realloc
#else
#include "myalloc.h"
#endif

int main(void) {
    printf("--- Demo starting ---\n");

    /* 1. Утечка памяти (Leak) */
    char *leak_ptr = my_malloc(64);
    strcpy(leak_ptr, "leaked memory");
    /* my_free(leak_ptr); */

    /* 2. Double Free */
    char *df_ptr = my_malloc(32);
    my_free(df_ptr);
    my_free(df_ptr); /* Повторное освобождение */

    /* 3. Use After Free */
    char *uaf_ptr = my_malloc(32);
    my_free(uaf_ptr);
    printf("UAF data: %s\n", uaf_ptr); /* Чтение освобожденной памяти */

    printf("--- Demo finished ---\n");
    return 0;
}
