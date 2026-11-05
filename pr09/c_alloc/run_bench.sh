#!/bin/bash
gcc -O2 bench.c myalloc.c -o bench_mine
gcc -O2 -DUSE_GLIBC bench.c -o bench_glibc

echo "Running benchmarks..."
echo "Scenario | Strategy | Time (ms) | Peak RSS (KB)"
echo "----------------------------------------------"

for scen in small alt large; do
    # First fit / myalloc
    res=$(./bench_mine $scen)
    echo "$scen | first | $res"
done

for scen in small alt large; do
    # Glibc
    res=$(./bench_glibc $scen)
    echo "$scen | glibc | $res"
done

rm -f bench_mine bench_glibc
