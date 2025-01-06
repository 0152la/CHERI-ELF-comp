## Goal

To compartmentalise an executable compiled to a dynamic library file `foo.so`,
we want to create a contiguous area of memory, containing:

* the executable code required from `foo.so`, and any further libraries that
  `foo.so` depends on. For this, we must read the program headers of each
  dynamic library to parse, in order to know which binary segments are required
  to be placed in the eventual compartment. This is just copied as is from the
  binary files, and the only changes we make are updating the symbol addresses
  (relocation);
* a stack and a heap for compartment execution;
* additional "special" data, such as a dedicated `environ` region, or a
  dedicated `TLS` region.

## Phases

The process is split into a number of distinct phases:

* register a compartment
* map a compartment
* execute a compartment
* clean a compartment

The final two phases are rather straight-forward. Execution means finding a
valid entry point in the compartment, setting up the registers with appropriate
values inside to pass as arguments, and finally transitioning within the
compartment (updating the `DDC`, `SP`, `TPIDR` registers, then jumping to the
address of the function to call). Cleaning is done by un-mapping the
compartment, and cleaning up any information logged about the compartment.

The other two phases are slightly more complex, and are discussed in detail
following.

### Registering

This is mainly performed via `register_new_comp` in `manager.c`, and
`comp_from_elf` within `compartment.c` (called by the former).

In this phase, we mainly read the required data from all dynamic libraries to
be contained within the compartment, information to perform relocations, and
handling of special compartment data (e.g., TLS, and `environ`).

From the originally given dynamic library, we read the program headers to know
which segments we will need to eventually map, as well as all defined symbols
and relocations. All these components are given an address that they will be
mapped to, according to the information contained in the ELF data.

**CHANGE** in the old implementation, each compartment had a set address to be
mapped at, so we could compute the address of each segment relative to this
base address. In the new implementation, as we do not know the eventually
address we will map to, we only set an offset at which each element will
eventually be mapped to.

We read all dependencies from `foo.so` (contained in the ELF information), and
repeat the process. At this point, we now know all symbols, all relocations,
and all segments that need to be mapped. We determine the amount of memory
required by the special compartment memory regions (TLS and `environ`),
identify the compartment entry points (for simplicity, this is just `main`),
and finally resolve all relocations, by finding the appropriate symbols across
all library symbols.

At this point, the "compartment" does not exist. This is just information
required to bring the compartment to existence.

**CHANGE** In the new implementation, we now stage this compartment, which
involves a `mmap` of one compartment size. It is not yet executable, as the
relocations and symbol addresses are only relative to the eventual base mapping
address. We stage the entirety of the compartment, including stack and heap
(**POTENTIAL OPTIMISATION** we don't need to stage the map and heap at this
point, but this does not affect the problematic `memcpy`)

### Mapping

**CHANGE** This is performed in different places in the implementations. The
old implementation has this happening in `comp_map` in `compartment.c`. The new
implementation has it in `mapping_new_fixed` in `manager.c`. Also, `stage_comp`
in `compartment.c` is theoretically equivalent to the old `comp_map`.
Essentially, the work done by the old `comp_map` has now been split into these
two new functions.

To make the compartment we prepared executable, we now map it into an area we
compartmentalise. We `mmap` enough space for the compartment data.

**CHANGE** The address at which we `mmap` is different. In the old
implementation, this is arbitrarily set, starting with the first compartment at
`0x1000000`. In the new implementation, this is left to the system allocator to
be decided.

**CHANGE** In the old implementation, here we `mmap` enough just for the binary
segment data needed to be copied. A future `mmap` will allocate space for the
special memory regions, stack, and heap. In the new implementation, we `mmap`
it all at once.

We then read the binary segment data into the mapped region.

**CHANGE** This is a first major divergence point. In the old implementation,
this was done via `pread`, by opening each file and reading from the
filesystem. The main reason for the rewrite was that half of the system call
time was spent on these `pread`s, as well as having to perform them for every
single compartment instantiation. In the new implementation, this data is
gotten from copying the staged compartment. This copy is the problematic
`memcpy`

**CHANGE** In the previous implementation, the protection flags of all mapped
segments was set to most permissive (`PROT_READ | PROT_EXEC | PROT_WRITE`) with
a TODO. This was fixed in the new implementation, by iterating over the
segments and using `mprotect` to update the flags appropriately. This
introduces additional overhead, but very small, and nothing compared to the
problematic `memcpy`.

We allocate space for scratch memory, comprising of the special memory areas,
stack, and heap.

**CHANGE** As mentioned above, this is a separate `mmap` in the previous
implementation. This is contained within the first `mmap` for the new
implementation.

We iterate over every relocation entry to update the address to the correct
symbol identified. Further, we update the pointers in the `environ` special
memory section from relative addresses to absolute addresses.

**CLARIFICATION** This happens for both implementations.

At this point, the compartment is completed and ready for execution.
