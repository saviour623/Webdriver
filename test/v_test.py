import sys, math

def segment(a) -> int:
    s = a >> 8
    s = s + bool(a - (s << 8))

    return s

def position(a) -> int:
    p = segment(a)
    s = p >> 1

    v = (1 - (p - (0.00390625 * a))) * 256

    return v
#(a - 256s) - (256p - 256x))
#256 * (a/256 - s)
def main():
    a = int(input("split block size: "))
    print(f"block [{segment(a)}]\nposition [{position(a)}]")

main()

