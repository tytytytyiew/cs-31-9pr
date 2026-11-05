#ifndef MYALLOC_H
#define MYALLOC_H

#include <stddef.h>

void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_realloc(void *ptr, size_t size);

typedef enum {
    STRATEGY_FIRST_FIT,
    STRATEGY_BEST_FIT
} alloc_strategy_t;

void my_set_strategy(alloc_strategy_t strategy);

#endif
