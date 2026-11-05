import random

random.seed(42)
live, n_pid, lines, allocs, frees = [], 0, [], 0, 0
while len(lines) < 200:
    want_alloc = (allocs < 113) and (frees >= 87 or not live or random.random() < 0.565)
    if want_alloc:
        n_pid += 1
        pid = f"Q{n_pid}"
        lines.append(f"alloc {pid} {random.randint(8, 298)}")
        live.append(pid)
        allocs += 1
    elif live:
        pid = live.pop(random.randrange(len(live)))
        lines.append(f"free {pid}")
        frees += 1
open("ops2.txt", "w").write("\n".join(lines) + "\n")
print(allocs, "alloc,", frees, "free")
