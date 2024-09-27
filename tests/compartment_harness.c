#define __capability

#include "compartment.h"

extern char** environ;
char ** proc_env_ptr;
void* __capability sealed_redirect_cap = NULL;

// XXX Should be unused
const unsigned short avg_sz_per_env_entry = 128;
const unsigned short max_env_count = 128;
const size_t max_env_sz
    = max_env_count * sizeof(char *) + avg_sz_per_env_entry * max_env_count;

int64_t
comp_exec_in(void * comp_sp, void *__capability comp_ddc, void * fn, void *args, size_t args_count, void *__capability src, void * tls)
{
    return (int64_t) fn;
}

int
main(int argc, char** argv)
{
    assert(argc == 2
        && "Expect at least one argument: binary file for compartment");
    char *file = argv[1];

    proc_env_ptr = environ;

    struct CompEntryPointDef* mock_cep = malloc(sizeof(struct CompEntryPointDef));
    mock_cep->name = malloc(strlen("main") + 1);
    strcpy((char *) mock_cep->name, "main");
    mock_cep->arg_count = 0;
    mock_cep->args_type = NULL;

    struct CompConfig* mock_cc = malloc(sizeof(struct CompConfig));
    mock_cc->heap_size = 0x800000UL;
    mock_cc->stack_size = 0x80000UL;
    mock_cc->entry_points = mock_cep;
    mock_cc->entry_point_count = 1;
    mock_cc->base_address = malloc(sizeof(char));

    struct Compartment *hw_comp = comp_from_elf(file, mock_cc);

    comp_clean(hw_comp);
    return 0;
}
