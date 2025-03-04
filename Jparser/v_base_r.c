
#include <stdio.h>
#define P 0
#define P1 1
#define T(...) # __VA_ARGS__

#define VEC_new(size_t_sz, ...) VEC_create(size_t_sz, # __VA_ARGS__ "[")

int main() {
    VEC_new(0)
}