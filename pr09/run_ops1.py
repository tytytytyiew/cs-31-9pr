import sys
from memsim import load, run

strategy = sys.argv[1] if len(sys.argv) > 1 else "first"
coalesce = "merge" in sys.argv[2:]
mm = run(load("ops1.txt"), strategy, coalesce=coalesce, verbose=True)
tot, mx, holes = mm.stats()
print(f"свободно={tot} макс. дыра={mx} дыр={holes} отказов={mm.refused}")
