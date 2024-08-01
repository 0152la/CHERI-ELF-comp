#include <assert.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
    const char *hw = "Hello World!";

    char *x = malloc(strlen(hw) + 1);
    strcpy(x, hw);
    assert(!strcmp(x, "Hello World!"));

    x = realloc(x, strlen(hw) * 2 + 1);
    strcat(x, hw);
    assert(!strcmp(x, "Hello World!Hello World!"));

    x = realloc(x, strlen(hw) + 1);
    x[strlen(hw)] = '\0';
    assert(!strcmp(x, "Hello World!"));

    free(x);

    const size_t malloc_block_sz = 16;

    // Check free
    void* tmp01 = malloc(2 * malloc_block_sz);
    void* tmp02 = malloc(1 * malloc_block_sz);
    free(tmp01);
    tmp01 = malloc(2 * malloc_block_sz);
    free(tmp02);
    free(tmp01);

    // Check finding right block to insert
    void* tmp1 = malloc(1 * malloc_block_sz);
    void* y = malloc(3 * malloc_block_sz);
    void* tmp2 = malloc(1 * malloc_block_sz);
    void* tmp3 = malloc(5 * malloc_block_sz);
    free(y);
    void* tmp4 = malloc(malloc_block_sz);
    void* tmp5 = malloc(malloc_block_sz);

    assert(tmp1 < tmp4);
    assert(tmp1 < tmp5);
    assert(tmp4 < tmp2);
    assert(tmp5 < tmp2);

    free(tmp1);
    free(tmp2);
    free(tmp3);
    free(tmp4);
    free(tmp5);

    return 0;
}
