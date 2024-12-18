#include "manager.h"

int
main(int argc, char **argv)
{
    size_t all_bench = bench_init("all");
    bench_start(all_bench);
    // Initial setup
    manager_ddc = cheri_ddc_get();
    BENCH(setup_intercepts(), "setup");

    assert(argc >= 2
        && "Expect at least one argument: binary file for compartment");
    char *file = argv[1];

    struct Compartment *hw_comp;
    BENCH(hw_comp = register_new_comp(file, true), "register");
    struct CompMapping *hw_map;
    BENCH(hw_map = mapping_new(hw_comp), "map");
    int comp_result = mapping_exec(hw_map, "main", NULL);
    mapping_free(hw_map);
    comp_clean(hw_comp);

    bench_end(all_bench);

    return comp_result;
}
