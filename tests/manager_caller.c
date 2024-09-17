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
    comp_map(hw_comp);
    int comp_result;
    BENCH(comp_result = exec_comp(hw_comp, "main", NULL), "exec");
    comp_clean(hw_comp);

    bench_end(all_bench);

    return comp_result;
}
