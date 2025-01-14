#ifndef _COMPARTMENT_DATA_H
#define _COMPARTMENT_DATA_H

#include "compartment_common.h"
#include "compartment_exec.h"

/**
 * Struct representing the memory of a data compartment. This means the
 * scratch memory part, comprising of the stack, the heap, and additional
 * special sections, such as `environ` or TLS.
 *
 * A DataCompartment must be initialised from an existing ExecCompartment.
 */
struct DataCompartment
{
    struct ExecCompartment* ec;
    size_t data_size;

    size_t stack_size;
    size_t heap_size;

    // TLS
    //struct TLSDesc* libs_tls_sects;
    //size_t total_tls_size;

    size_t page_size;
};

struct DataCompartment*
make_data_comp(struct ExecCompartment*, struct CompConfig*);

size_t get_dc_tls_offset(struct DataCompartment*);
size_t get_dc_stack_offset(struct DataCompartment*);
size_t get_dc_heap_offset(struct DataCompartment*);

#endif // _COMPARTMENT_DATA_H
