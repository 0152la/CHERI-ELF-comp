#ifndef _COMPARTMENT_H
#define _COMPARTMENT_H

#include <assert.h>
#include <elf.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <fts.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/auxv.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// TODO consider re-organizing
#include "symbols.h"

#ifdef __CHERI__
#include "cheriintrin.h"
#endif

// Morello `gcc` defines `ptraddr_t` in `stddef.h`, while `clang` does so in
// `stdint.h`
// TODO are there any other diverging definition files?
#if defined(__GNUC__) && !defined(__clang__)
#include <stddef.h>
#endif

#include "compartment_common.h"

// TODO once things stabilize, recheck if all struct members are required
// currently there's quite a bit of redundancy to make things easier to think
// about

int64_t
comp_exec_in(void *, void *__capability, void *, void *, size_t,
    void *__capability, void *);
void
comp_exec_out();

// Declare built-in function for cache synchronization:
// https://community.arm.com/arm-community-blogs/b/architectures-and-processors-blog/posts/caches-and-self-modifying-code
extern void
__clear_cache(void *, void *);

// Number of instructions required by the transition function
extern void *__capability sealed_redirect_cap;
extern void *__capability comp_return_caps[2];

// Maximum size of an argument, in bytes
#define COMP_ARG_SIZE 8

// This is a guard for the expected size of an argument, and a consequence of
// using `x` registers in `loading_params` in `transition.S`. This should be
// the equivalent of checking for a 64-bit CHERI aware platform
// TODO is there a better way to check?
// TODO recheck if we need this
#if !defined(CHERI_COMP_LINUX) && !(__LP64__ && defined(__CHERI__))
#error Expecting 64-bit Arm Morello platform
#endif

// Default sizes for compartment heap and stack, if not explicitly given
#define DEFAULT_COMP_HEAP_SZ 0x800000UL // 800kB
#define DEFAULT_COMP_STACK_SZ 0x80000UL // 80kB

/* Struct representing one segment of an ELF binary.
 *
 * TODO expand */
struct SegmentMap
{
    void *mem_bot;
    size_t offset;
    size_t mem_sz;
    size_t file_sz;
    int prot_flags;
};

/* Struct representing an eager relocation to be made at map-time - instead of
 * lazily looking up function addresses once a function is called at runtime,
 * via PLT/GOT, we update the expected addresses eagerly once the code is
 * mapped into memory
 */
struct LibRelaMapping
{
    char *rela_name;
    void *rela_address; // address of relocation in compartment
    void *target_func_address; // address of actual function
    unsigned short rela_type; // type of relocation
    unsigned short rela_sym_type; // type of underlying symbol
    unsigned short rela_sym_bind; // bind of underlying symbol
    uint16_t rela_sym_shndx; // section index of underlying symbol
    bool mapping_reloc; // whether to relocate when mapped for execution
};

/* Struct representing configuration data for one entry point; this is just
 * information that we expect to appear in the compartment, as given by its
 * compartment configuration file
 */
struct CompEntryPointDef
{
    char *name;
    size_t arg_count;
    char **args_type;
    void *comp_addr;
};

#include "compartment_exec.h"
#include "compartment_data.h"

/**
 * Struct representing ELF data necessary to load and eventually execute a
 * compartment
 */
struct Compartment
{
    struct ExecCompartment* ec;
    struct DataCompartment* dc;

    // Identifiers, manager by `manager.c`
    size_t id;
    struct CompConfig *cc;

    // Execution info
    // ELF data
    size_t total_size; // size of compartment in memory
    size_t data_size; // size of data segments of ELF files
    void *staged_addr; // address where compartment data is stored, ready for
                       // mapping

    // Environ
    char **environ_ptr;
    size_t environ_sz;

    // Scratch memory
    void *scratch_mem_base;
    size_t scratch_mem_size;
    size_t scratch_mem_extra;

    size_t scratch_mem_heap_size;
    void *scratch_mem_stack_top;
    size_t scratch_mem_stack_size;

    // Internal libraries and relocations
    size_t libs_count;
    struct LibDependency **libs;
    void *tls_lookup_func;
    size_t total_tls_size;
    struct TLSDesc *libs_tls_sects;
    comp_symbol_list *comp_syms;

    // Hardware info - maybe move
    size_t page_size;
};

int
entry_point_cmp(const void *, const void *);
struct Compartment *
comp_from_elf(char *, struct CompConfig *); // char **, size_t, void *);
void
comp_clean(struct Compartment *);
void *
get_seg_target(void *, struct LibDependency *, size_t);

struct Compartment *
find_comp(struct Compartment *);

#endif // _COMPARTMENT_H
