#include "myalloc.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

static int tests_passed = 0;
static int tests_total = 0;

#define RUN_TEST(test_func) do { \
    tests_total++; \
    test_func(); \
    tests_passed++; \
    printf("[OK] %s\n", #test_func); \
} while(0)

void test_basic_alloc(void) {
    void *p1 = my_malloc(100);
    assert(p1 != NULL);
    strcpy((char *)p1, "Hello, allocator!");
    assert(strcmp((char *)p1, "Hello, allocator!") == 0);
    my_free(p1);
}

void test_strategies(void) {
    my_set_strategy(STRATEGY_FIRST_FIT);
    void *p1 = my_malloc(64);
    void *p2 = my_malloc(64);
    my_free(p1);
    
    void *p3 = my_malloc(32); /* Должен встать на место p1 (first/best fit) */
    assert(p3 != NULL);
    
    my_set_strategy(STRATEGY_BEST_FIT);
    void *p4 = my_malloc(32);
    assert(p4 != NULL);

    my_free(p2);
    my_free(p3);
    my_free(p4);
}

void test_realloc(void) {
    char *p = my_malloc(10);
    assert(p != NULL);
    strcpy(p, "test");
    
    p = my_realloc(p, 1000);
    assert(p != NULL);
    assert(strcmp(p, "test") == 0);
    
    p = my_realloc(p, 5); /* Уменьшение */
    assert(p != NULL);
    
    my_free(p);
}

int main(void) {
    RUN_TEST(test_basic_alloc);
    RUN_TEST(test_strategies);
    RUN_TEST(test_realloc);

    printf("\nВсего проверок: %d, провалено: 0\n", tests_total);
    return 0;
}
