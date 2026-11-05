MEMORY = 1024


class Block:
    def __init__(self, start, size, pid=None):
        self.start, self.size, self.pid = start, size, pid

    @property
    def free(self):
        return self.pid is None


class MemoryManager:
    def __init__(self, strategy="first", coalesce=False):
        self.blocks = [Block(0, MEMORY)]
        self.strategy = strategy
        self.coalesce = coalesce
        self.refused = 0

    def candidates(self, size):
        return [b for b in self.blocks if b.free and b.size >= size]

    def choose(self, size):
        cands = self.candidates(size)
        if not cands:
            return None
        if self.strategy == "first":
            return cands[0]
        if self.strategy == "best":
            return min(cands, key=lambda b: b.size)
        if self.strategy == "worst":
            return max(cands, key=lambda b: b.size)
        raise ValueError("unknown strategy: " + self.strategy)

    def alloc(self, pid, size):
        block = self.choose(size)
        if block is None:
            self.refused += 1
            return False
        if block.size > size:
            idx = self.blocks.index(block)
            rest = Block(block.start + size, block.size - size)
            self.blocks.insert(idx + 1, rest)
            block.size = size
        block.pid = pid
        return True

    def free(self, pid):
        for b in self.blocks:
            if b.pid == pid:
                b.pid = None
                if self.coalesce:
                    self._merge()
                return True
        return False

    def _merge(self):
        merged = []
        for b in self.blocks:
            if merged and merged[-1].free and b.free:
                merged[-1].size += b.size
            else:
                merged.append(b)
        self.blocks = merged

    def show(self):
        return " ".join("[%s:%d]" % (b.pid or "-", b.size) for b in self.blocks)

    def holes(self):
        return [b.size for b in self.blocks if b.free]

    def fragmentation(self):
        h = self.holes()
        if not h or sum(h) == 0:
            return 0.0
        return 1 - max(h) / sum(h)

    def stats(self):
        h = self.holes()
        return {
            "holes": len(h),
            "free_total": sum(h),
            "largest": max(h) if h else 0,
            "fragmentation": round(self.fragmentation(), 3),
            "refused": self.refused,
        }
