from memsim import MemoryManager, load
from buddy import Buddy


def simulate(make, alloc, free, stats, ops):
    m, ext, internal = make(), [], []
    for cmd, pid, *rest in ops:
        alloc(m, pid, int(rest[0])) if cmd == "alloc" else free(m, pid)
        tot, mx, extra = stats(m)
        ext.append(1 - mx / tot if tot else 0)  # доля свободной памяти вне самой большой дыры
        internal.append(extra)
    return m.refused, sum(ext) / len(ext), sum(internal) / len(internal)


def fit(strategy):
    def stats(m):
        tot, mx, holes = m.stats()
        return tot, mx, 0
    return (lambda: MemoryManager(strategy, coalesce=True),
            lambda m, p, s: m.alloc(p, s),
            lambda m, p: m.free(p),
            stats)


def buddy():
    return (lambda: Buddy(),
            lambda m, p, s: m.alloc(p, s),
            lambda m, p: m.release(p),
            lambda m: m.stats())


if __name__ == "__main__":
    ops = load("ops2.txt")
    print(f"{'подход':<14}{'отказов':>8}{'внешняя фр.':>13}{'внутренняя':>12}")
    rows = [(f"{s} +merge", simulate(*fit(s), ops)) for s in ("first", "best", "worst")]
    rows.append(("buddy", simulate(*buddy(), ops)))
    for name, (ref, ext, inn) in rows:
        print(f"{name:<14}{ref:>8}{ext:>13.2f}{inn:>12.0f}")
