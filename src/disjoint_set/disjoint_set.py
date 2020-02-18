import sys
fast_input = lambda : sys.stdin.readline().strip()

class DisjointSet:
    """
    Disjoint Set Union. 
    Supports merging and status checking.
    """

    def __init__(self, size: int):
        self.parent = [i for i in range(size)]
        self.rank = [1 for i in range(size)]

    def root(self, v: int) -> int:
        """
        Return root of v.
        """
        if v == self.parent[v]: return v
        self.parent[v] = self.root(self.parent[v])
        return self.parent[v]
    
    def is_disjoint(self, v1: int, v2: int) -> bool:
        """
        Return if v1 and v2 are disjoint.
        """
        return self.root(v1) != self.root(v2)

    def merge(self, v1: int, v2: int):
        """
        Merge v1 and v2 using rank.
        """
        root1, root2 = self.root(v1), self.root(v2)
        if root1 == root2: return
        elif self.rank[root1] >= self.rank[root2]:
            self.parent[root2] = root1
            self.rank[root1] += 1
        else:
            self.parent[root1] = root2
            self.rank[root2] += 1


if __name__ == "__main__":
    
    n, m = [int(c) for c in fast_input().split()]
    DSU = DisjointSet(n+1)
    for qi in range(m):
        command, v1, v2 = [int(c) for c in fast_input().split()]
        if command == 0:
            DSU.merge(v1, v2)
        else:
            print("YES" if not DSU.is_disjoint(v1, v2) else "NO")