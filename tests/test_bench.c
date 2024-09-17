#include <stdlib.h>

#include "benchmarking.h"

int
main(void)
{
    void* x;
    BENCH(x = malloc(16), "malloc");
    free(x);

    const unsigned short xs_count = 10;
    void* xs[xs_count];
    for (size_t i = 0; i < xs_count; ++i)
    {
        BENCH(xs[i] = malloc(1024), "mallocxs");
        printf("xs[%zu] = %p\n", i, xs[i]);
        assert(xs[i]);
    }
    for (size_t i = 0; i < xs_count; ++i)
    {
        BENCH(free(xs[i]), "freexs");
    }
}
