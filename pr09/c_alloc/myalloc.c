#define _GNU_SOURCE
#include "myalloc.h"
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define HEAP_SIZE (256 * 1024 * 1024) /* 256 MB */
#define MAGIC_FREE 0x46524545         /* 'FREE' */
#define MAGIC_USED 0x55534544         /* 'USED' */

typedef struct block {
    size_t size;          /* Размер полезной нагрузки */
    int is_free;          /* Флаг свободы */
    uint32_t magic;       /* Магическое число для проверки */
    struct block *next;
    struct block *prev;
} block_t;

#define BLOCK_HEADER_SIZE sizeof(block_t)

static void *heap_start = NULL;
static block_t *free_list = NULL;
static alloc_strategy_t current_strategy = STRATEGY_FIRST_FIT;

static void init_heap(void) {
    if (heap_start) return;
    
    heap_start = mmap(NULL, HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (heap_start == MAP_FAILED) {
        perror("mmap failed");
        _exit(1);
    }
    
    free_list = (block_t *)heap_start;
    free_list->size = HEAP_SIZE - BLOCK_HEADER_SIZE;
    free_list->is_free = 1;
    free_list->magic = MAGIC_FREE;
    free_list->next = NULL;
    free_list->prev = NULL;
}

void my_set_strategy(alloc_strategy_t strategy) {
    current_strategy = strategy;
}

void *my_malloc(size_t size) {
    if (size == 0) return NULL;
    if (!heap_start) init_heap();

    /* Выравнивание по 8 байт */
    size = (size + 7) & ~7;

    block_t *best_block = NULL;
    block_t *curr = free_list;

    if (current_strategy == STRATEGY_FIRST_FIT) {
        while (curr) {
            if (curr->is_free && curr->size >= size) {
                best_block = curr;
                break;
            }
            curr = curr->next;
        }
    } else { /* BEST_FIT */
        size_t min_remainder = (size_t)-1;
        while (curr) {
            if (curr->is_free && curr->size >= size) {
                size_t remainder = curr->size - size;
                if (remainder < min_remainder) {
                    min_remainder = remainder;
                    best_block = curr;
                    if (remainder == 0) break; /* Идеальное совпадение */
                }
            }
            curr = curr->next;
        }
    }

    if (!best_block) return NULL; /* Нет подходящего блока */

    /* Разбиение блока, если остаток позволяет создать новый блок */
    if (best_block->size >= size + BLOCK_HEADER_SIZE + 8) {
        block_t *new_block = (block_t *)((char *)best_block + BLOCK_HEADER_SIZE + size);
        new_block->size = best_block->size - size - BLOCK_HEADER_SIZE;
        new_block->is_free = 1;
        new_block->magic = MAGIC_FREE;
        
        new_block->next = best_block->next;
        new_block->prev = best_block;
        if (new_block->next) {
            new_block->next->prev = new_block;
        }
        best_block->next = new_block;
        best_block->size = size;
    }

    best_block->is_free = 0;
    best_block->magic = MAGIC_USED;

    return (void *)((char *)best_block + BLOCK_HEADER_SIZE);
}

void my_free(void *ptr) {
    if (!ptr) return;

    block_t *block = (block_t *)((char *)ptr - BLOCK_HEADER_SIZE);
    if (block->magic != MAGIC_USED || block->is_free) {
        fprintf(stderr, "[myalloc] double free or corruption detected\n");
        return;
    }

    block->is_free = 1;
    block->magic = MAGIC_FREE;

    /* Слияние со следующим блоком */
    if (block->next && block->next->is_free) {
        block->size += BLOCK_HEADER_SIZE + block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }
    }

    /* Слияние с предыдущим блоком */
    if (block->prev && block->prev->is_free) {
        block->prev->size += BLOCK_HEADER_SIZE + block->size;
        block->prev->next = block->next;
        if (block->next) {
            block->next->prev = block->prev;
        }
    }
}

void *my_realloc(void *ptr, size_t size) {
    if (!ptr) return my_malloc(size);
    if (size == 0) {
        my_free(ptr);
        return NULL;
    }

    block_t *block = (block_t *)((char *)ptr - BLOCK_HEADER_SIZE);
    if (block->magic != MAGIC_USED) return NULL;

    size = (size + 7) & ~7;

    if (block->size >= size) {
        return ptr; /* Текущий блок достаточен */
    }

    /* Попробуем расширить за счет следующего свободного блока */
    if (block->next && block->next->is_free && 
        (block->size + BLOCK_HEADER_SIZE + block->next->size >= size)) {
        
        block->size += BLOCK_HEADER_SIZE + block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }
        return ptr;
    }

    /* Иначе стандартный путь: выделить новый, скопировать, освободить старый */
    void *new_ptr = my_malloc(size);
    if (!new_ptr) return NULL;

    memcpy(new_ptr, ptr, block->size < size ? block->size : size);
    my_free(ptr);
    return new_ptr;
}
