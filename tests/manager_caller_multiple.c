#include "manager.h"

int
main(int argc, char **argv)
{
    size_t b_all = bench_init("all");
    bench_start(b_all);
    const char *count_env_name = "EXECUTE_COUNT";
    const char *count_env_val = getenv(count_env_name);
    const unsigned int comps_count_default = 100;
    unsigned int comps_count
        = count_env_val ? atoi(count_env_val) : comps_count_default;

    // Initial setup
    manager_ddc = cheri_ddc_get();
    BENCH(setup_intercepts(), "intercepts");

    assert(argc >= 2
        && "Expect at least one argument: binary file for compartment");
    char *file = argv[1];

    struct Compartment* hw_comp;
    BENCH(hw_comp = register_new_comp(file, true), "register");
    struct CompMapping *hw_map;
    int comp_result = 0;
    size_t b_map = bench_init("mapping");
    bench_start(b_map);
    for (size_t i = 0; i < comps_count; ++i)
    {
        BENCH(hw_map = mapping_new(hw_comp), "mapping-new");
        BENCH(comp_result = (mapping_exec(hw_map, "main", NULL) != 0) || comp_result, "mapping-exec");
        BENCH(mapping_free(hw_map), "mapping-free");
    }
    bench_end(b_map);
    BENCH(comp_clean(hw_comp), "clean");
    assert(!comp_result);
    bench_end(b_all);
    return comp_result;
}
