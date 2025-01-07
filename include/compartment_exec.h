/**
 * Struct representing the executable part of a compartment. This is where
 * code from required libraries will go.
 *
 * Our data compartmentalisation approach will have a single ExecCompartment
 * for multiple DataCompartments.
 */
struct ExecCompartment
{
    size_t exec_size;
    void* staged_addr;

    unsigned short libs_count;
    struct LibDependency** libs;

    // Address of function used to perform lookup of TLS variables
    void* tls_lookup_func;

    // Symbols contained within all libraries of this executable compartment
    comp_symbol_list *comp_syms;
}

