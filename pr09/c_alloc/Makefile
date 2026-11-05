CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2 -g

all: test demo_mine bench

test: test.c myalloc.c myalloc.h
	$(CC) $(CFLAGS) test.c myalloc.c -o test

demo_mine: demo.c myalloc.c myalloc.h
	$(CC) $(CFLAGS) demo.c myalloc.c -o demo_mine

bench: bench.c myalloc.c myalloc.h
	$(CC) $(CFLAGS) bench.c myalloc.c -o bench

clean:
	rm -f test demo_mine demo_glibc bench
