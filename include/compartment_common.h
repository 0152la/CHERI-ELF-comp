#ifndef _COMPARTMENT_COMMON_H
#define _COMPARTMENT_COMMON_H

#include <stddef.h>

#include "symbols_lib.h"

#define align_down(x, align) __builtin_align_down(x, align)
#define align_up(x, align) __builtin_align_up(x, align)

/* Struct representing a compartment configuration.
 */
struct CompConfig
{
    size_t heap_size;
    size_t stack_size;
    struct CompEntryPointDef *entry_points;
    size_t entry_point_count;

    // Variables related to `manager.h` prepared `environ` data
    char **env_ptr; // pointer to `environ` array
    size_t env_ptr_sz; // size of the array
                       // TODO might be unneeded
    unsigned short env_ptr_count; // number of entries

    size_t page_size;
};

/**
 * Struct representing a library dependency for one of our given compartments
 */
struct LibDependency
{
    char *lib_name;
    char *lib_path;
    void *lib_mem_base; // relative address in compartment

    void *data_base; // address of data mapped in loader
    size_t data_size; // size of mapped data

    // Segments of interest (usually, of type `PT_LOAD`) within this library
    size_t lib_segs_count;
    size_t lib_segs_size;
    struct SegmentMap *lib_segs;

    // Symbols within this library
    lib_symbol_list *lib_syms;

    // Library dependencies for this library
    unsigned short lib_dep_count;
    char **lib_dep_names;

    // Symbols within this library that need eager relocation
    size_t rela_maps_count;
    struct LibRelaMapping *rela_maps;

    // TLS-related variables
    // TODO can there be more TLS sections?
    void *tls_sec_addr;
    size_t tls_sec_off;
    size_t tls_sec_size;
    size_t tls_data_size;
    // offset from TLS base pointer (i.e., value of `tpidr_el0`) where this
    // library's TLS variables start
    size_t tls_offset;
};

/**
 * Struct representing TLS information for a compartment. Since we currently
 * enforce only single-threaded code, we basically only have pointers for
 * regions allocated for TLS for each dynamic shared object
 */
struct TLSDesc
{
    unsigned short region_count;
    size_t region_size;
    void *region_start;
    unsigned short libs_count;
};

#endif // _COMPARTMENT_COMMON_H
