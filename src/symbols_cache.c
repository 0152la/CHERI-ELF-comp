#include "symbols_cache.h"

cache_symbol_list *
cache_syms_init()
{
    cache_symbol_list *new_list = malloc(sizeof(cache_symbol_list));
    new_list->data_count = 0;
    new_list->data = NULL;
    return new_list;
}

void
cache_syms_clean(cache_symbol_list *list)
{
    for (size_t i = 0; i < list->data_count; ++i)
    {
        comp_syms_clean(list->data[i]->candidates);
        free(list->data[i]);
    }
    free(list->data);
    free(list);
}

void
cache_syms_insert(const char* sym_name, comp_symbol_list* candidates, cache_symbol_list *list)
{
    cache_symbol* new_cs = malloc(sizeof(cache_symbol));
    new_cs->sym_name = sym_name;
    new_cs->candidates = candidates;

    size_t curr_count = list->data_count;
    list->data = realloc(list->data, (curr_count + 1) * sizeof(cache_symbol *));

    if (list->data == NULL || new_cs == NULL)
    {
        err(1, "Error inserting symbol %s in cache_list!", sym_name);
    }

    list->data[curr_count] = new_cs;
    list->data_count += 1;
}

cache_symbol*
cache_syms_search(const char *to_find, const cache_symbol_list *list)
{
    for (size_t i = 0; i < list->data_count; ++i)
    {
        if (!strcmp(list->data[i]->sym_name, to_find))
        {
            return list->data[i];
        }
    }
    return NULL;
}

comp_symbol_list*
cache_syms_get_candidates(const cache_symbol* to_get)
{
    return to_get->candidates;
}
