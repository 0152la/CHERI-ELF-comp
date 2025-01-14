#include "compartment_data.h"

/*******************************************************************************
 * Forward declarations
 ******************************************************************************/

static void setup_environ(struct DataCompartment*, struct CompConfig*);
static void setup_tls_regions(struct DataCompartment*, struct ExecCompartment*);

static void adjust_dc_size(struct DataCompartment*, size_t);

/*******************************************************************************
 * Main compartment functions
 ******************************************************************************/

struct DataCompartment*
make_data_comp(struct ExecCompartment* ec, struct CompConfig* cc)
{
    struct DataCompartment* new_dc = malloc(sizeof(struct DataCompartment));
    new_dc->stack_size = cc->stack_size;
    new_dc->heap_size = cc->heap_size;
    new_dc->page_size = cc->page_size;

    if (ec->has_tls)
    {
        setup_tls_regions(new_dc, ec);
    }

    return new_dc;
}

/*******************************************************************************
 * Utility functions
 ******************************************************************************/

size_t
get_dc_tls_offset(struct DataCompartment* dc)
{
    struct DataCompartment* omit = dc;
    return 0x0;
}

size_t
get_dc_stack_offset(struct DataCompartment* dc)
{
    return get_dc_tls_offset(dc) + dc->total_tls_size;
}

size_t
get_dc_heap_offset(struct DataCompartment* dc)
{
    return get_dc_stack_offset(dc) + dc->stack_size;
}

/*******************************************************************************
 * Helper functions
 ******************************************************************************/

/*static void*/
/*setup_environ(struct DataCompartment *new_dc, struct CompConfig* cc)*/
/*{*/
    /*assert(cc->env_ptr != NULL); // TODO consider optional check*/
    /*new_dc->environ_size*/
        /*= align_up(cc->env_ptr_sz, new_dc->page_size);*/
    /*adjust_dc_size(new_dc, new_dc->environ_size);*/
/*}*/

static void
setup_tls_regions(struct DataCompartment* new_dc, struct ExecCompartment* ec)
{
    // TODO currently we only support one thread
    new_dc->libs_tls_sects = malloc(sizeof(struct TLSDesc));
    new_dc->libs_tls_sects->region_count = 1;
    new_dc->libs_tls_sects->region_start
        = (void*) get_dc_tls_offset(new_dc);
    new_dc->libs_tls_sects->libs_count = 0;

    size_t tls_size = 0;
    for (size_t i = 0; i < ec->libs_count; ++i)
    {
        if (ec->libs[i]->tls_sec_addr == 0x0)
        {
            continue;
        }

        ec->libs[i]->tls_offset = tls_size;
        tls_size += ec->libs[i]->tls_sec_size;
        new_dc->libs_tls_sects->libs_count += 1;
    }

    new_dc->total_tls_size = align_up(tls_size *
            new_dc->libs_tls_sects->region_count, new_dc->page_size);
    new_dc->libs_tls_sects->region_size = tls_size;
    adjust_dc_size(new_dc, new_dc->total_tls_size);
}

static void
adjust_dc_size(struct DataCompartment* dc, size_t to_adjust)
{
    assert(to_adjust % dc->page_size == 0);
    dc->data_size += to_adjust;
}
