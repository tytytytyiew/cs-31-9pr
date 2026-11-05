#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

#ifdef USE_GLIBC
#define my_malloc malloc
#define my_free free
#define my_realloc realloc
#else
#include "myalloc.h"
#endif

#define NUM_OPS 10000

void bench_small_allocs(void) {
    void *ptrs[1000];
    for (int i = 0; i < 1000; i++) {
        ptrs[i] = my_malloc(16);
    }
    for (int i = 0; i < 1000; i++) {
        my_free(ptrs[i]);
    }
}

void bench_alternating(void) {
    void *ptrs[500] = {0};
    for (int i = 0; i < NUM_OPS; i++) {
        int idx = rand() % 500;
        if (ptrs[idx]) {
            my_free(ptrs[idx]);
            ptrs[idx] = NULL;
        } else {
            ptrs[idx] = my_malloc((rand() % 128) + 1);
        }
    }
    for (int i = 0; i < 500; i++) {
        if (ptrs[i]) my_free(ptrs[i]);
    }
}

void bench_large_blocks(void) {
    void *ptrs[50];
    for (int i = 0; i < 50; i++) {
        ptrs[i] = my_malloc(1024 * 1024); /* 1 MB */
    }
    for (int i = 0; i < 50; i++) {
        my_free(ptrs[i]);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    
    clock_t start = clock();
    
    if (strcmp(argv[1], "small") == 0) {
        for (int i = 0; i < 500; i++) bench_small_allocs();
    } else if (strcmp(argv[1], "alt") == 0) {
        bench_alternating();
    } else if (strcmp(argv[1], "large") == 0) {
        for (int i = 0; i < 200; i++) bench_large_blocks();
    }
    
    clock_t end = clock();
    double time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;

    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    
    printf("Time: %.2f ms, Peak RSS: %ld KB\n", time_ms, usage.ru_maxrss);
    return 0;
}
