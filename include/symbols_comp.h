#ifndef _CHERICOMP_SYMBOLS_COMP_H
#define _CHERICOMP_SYMBOLS_COMP_H

#include "symbols_lib.h"

struct simple_cs_list
{
    struct CompSymbol **data;
    size_t data_count;
};

typedef struct simple_cs_list comp_symbol_list;
typedef struct CompSymbol comp_symbol;

/* Struct representing a wrapper around a LibDependencySymbol, in order to
 * facilitate compartment-level searching
 */
struct CompSymbol
{
    struct LibDependencySymbol *sym_ref;
    size_t sym_lib_idx;
};

comp_symbol_list *
comp_syms_init();
void
comp_syms_clean(comp_symbol_list *);
void
comp_syms_clean_deep(comp_symbol_list *);
void
comp_syms_insert(comp_symbol *, comp_symbol_list *);
comp_symbol *
comp_syms_search(const char *, const comp_symbol_list *);
comp_symbol_list *
comp_syms_find_all(const char *, const comp_symbol_list *);

#endif // _CHERICOMP_SYMBOLS_COMP_H
