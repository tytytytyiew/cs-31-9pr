import sys
from memsim import MemoryManager


def run(strategy, filename="ops1.txt", coalesce=True, verbose=True):
    mm = MemoryManager(strategy, coalesce=coalesce)
    with open(filename) as f:
        lines = [l.split() for l in f if l.strip()]
    for i, parts in enumerate(lines, 1):
        if parts[0] == "alloc":
            ok = mm.alloc(parts[1], int(parts[2]))
            head = "%2d alloc %-3s %4s" % (i, parts[1], parts[2])
            tail = mm.show() + ("" if ok else "   REFUSED")
        else:
            mm.free(parts[1])
            head = "%2d free  %-3s     " % (i, parts[1])
            tail = mm.show()
        if verbose:
            print(head, " ", tail)
    return mm


if __name__ == "__main__":
    strategy = sys.argv[1] if len(sys.argv) > 1 else "first"
    mm = run(strategy)
    print()
    print(strategy, mm.stats())
