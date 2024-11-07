#include "compartment.c"
#include "manager.h"

int
main()
{
    manager_ddc = cheri_ddc_get();
    setup_intercepts();

    char *file = "./simple.so";
    struct Compartment *hw_comp = register_new_comp(file, true);
    struct CompMapping* hw_map = mapping_new(hw_comp);
    mapping_free(hw_map);
    comp_clean(hw_comp);
    return 0;
}
