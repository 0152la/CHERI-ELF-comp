/**
 * Struct representing the memory of a data compartment. This means the
 * scratch memory part, comprising of the stack, the heap, and additional
 * special sections, such as `environ` or TLS.
 *
 * A DataCompartment must be initialised from an existing ExecCompartment.
 */
struct DataCompartment
{
    struct ExecCompartment* exec_comp;
    size_t data_size;

    size_t stack_size;
    size_t heap_size;

    // Environ
    char** environ_ptr;
    size_t environ_sz;

    // TLS
    struct TLSDesc* libs_tls_sects;
}
