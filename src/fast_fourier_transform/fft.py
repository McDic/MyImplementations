import math

def fft(poly: list, invert: bool):

    # Resize
    n, log = len(poly), 0
    while (1<<log) < n: log += 1
    poly += [0 for _ in range((1<<log) - n)]
    n = 1<<log

    # Index swap
    def reverse_bit(index) -> int:
        return sum(1<<(log-1-b) \
            for b in range(log) if index & (1<<b))
    for i in range(n):
        j = reverse_bit(i)
        if i < j: poly[i], poly[j] = poly[j], poly[i]
    
    # Calculate
    inversefactor = -2 if invert else 2
    l = 2
    while l <= n:
        angle = inversefactor * math.pi / l
        wfactor = math.cos(angle) + math.sin(angle) * 1j
        for offset in range(0, n, l):
            w = 1+0j
            for i in range(l//2):
                front, back = poly[offset+i], poly[offset+i+l//2] * w
                poly[offset+i], poly[offset+i+l//2] = front+back, front-back
                w *= wfactor
        l <<= 1

    # Invert    
    if invert:
        for i in range(n): poly[i] /= n


def multiply(poly1: list, poly2: list):

    # Resize
    len1, len2 = len(poly1), len(poly2)
    n = 1
    while n < len1 + len2: n <<= 1
    poly1copy = poly1 + [0] * (n - len1)
    poly2copy = poly2 + [0] * (n - len2)

    # FT -> Convolution -> IFT
    fft(poly1copy, False)
    fft(poly2copy, False)
    for i in range(n): poly1copy[i] *= poly2copy[i]
    fft(poly1copy, True)

    # Return
    return [x.real for x in poly1copy]


n = int(input())
a = [int(c) for c in input().split(" ")]
b = [int(c) for c in input().split(" ")][::-1]
ab = [round(c) for c in multiply(a, b)]
ab.append(0)
print(max(ab[i] + ab[i+n] for i in range(n)))