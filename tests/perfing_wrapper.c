#include "perfing.h"

#include <assert.h>
#include <unistd.h>

int
main(int argc, char** argv)
{
    record_time("PRE_NATIVE_COMP_CALL");
    if (!execv(argv[0], &argv[1]))
    {
        assert(false);
    }
    record_time("POST_NATIVE_COMP_CALL");
    print_interval("PRE_NATIVE_COMP_CALL", "POST_NATIVE_COMP_CALL");
    return 0;
}
