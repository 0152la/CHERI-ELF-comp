#ifndef _CHERICOMP_SYMBOLS_CACHE_H
#define _CHERICOMP_SYMBOLS_CACHE_H

#include "symbols_comp.h"

struct simple_cache_list
{
    struct RelaCacheSymbol **data;
    size_t data_count;
};

struct RelaCacheSymbol
{
    const char* sym_name;
    comp_symbol_list* candidates;
};

typedef struct simple_cache_list cache_symbol_list;
typedef struct RelaCacheSymbol cache_symbol;

cache_symbol_list* cache_syms_init();
void cache_syms_clean(cache_symbol_list *);
void cache_syms_insert(const char*, comp_symbol_list*, cache_symbol_list *);
cache_symbol* cache_syms_search(const char *, const cache_symbol_list *);

comp_symbol_list* cache_syms_get_candidates(const cache_symbol*);


#endif // _CHERICOMP_SYMBOLS_CACHE_H
