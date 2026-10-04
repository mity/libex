
# `libex` Readme

Home: https://github.com/mity/libex


## Overview

This is a collection of assorted C utilities which can roughly be seen as an
extension of standard library, with the following properties:

 * **Highly reusable**
 * **No surprise**
 * **Minimal inner dependencies** so that projects can copy into their source
   tree only the stuff they need, without too much headache.


## Usage

### As any other library:

 1. Instruct your compiler to search the source directory
    (e.g. via `-Ipath/to/src`).

 2. Link your program with `libex` library.


### Direct reuse of relevant sources:

If your project needs only very little of the `libex` functionality, you may
prefer to directly copy the relervant `libex` sources and headers to your build
tree.

Note this is explicitly permitted; even without any accompanying file(s) such
as this `READMER.md` or `LICENSE.md` as long as you keep intact the copyright
and licensing notes at the beginning of all such source files.

(For this very purpose we try quite hard to keep the module inter-dependencies
to the required  minimum.)


## Compatibility Note

Please note we do **not** provide any formal compatibility guarantees, both on
the binary as well as source level, at least at the moment.

Of course we don't change any our interface willy-nilly unless there's strong
need for it, but we reserve any right to do so when such change is seemed
worth the pain caused to the users.


## Documentation

Documentation of all public types and functions is provided in comments
directly in the respective header files.


## Table of Contents

### Data Structures

 * **`buffer.[hc]`:** Simple growable/shrinkable buffer implementation.
   Requires `goodaloc.[hc]`.

 * **`rbtree.[hc]`:** Intrusive red-black tree implementation.

 * **`stack.h`:** Simple stack implementation, header-only wrapper of
   `ex/buffer.[hc]`.

 * **`list.h`:** Simple intrusive header-only implementation of double-linked
   lists, single-linked lists (S-lists) and single-linked lists with tail
   (Q-lists aka queue lists).

### Hash Functions and Cyclic Redundancy Check Functions

 * **`crc32.[hc]`:** CRC-32.

 * **`fnv1a_32.[hc]`:** 32-bit Fowler–Noll–Vo hash hash (variant FNV1a).

 * **`fnv1a_64.[hc]`:** 64-bit Fowler–Noll–Vo hash hash (variant FNV1a).

 * **`murmur3_32.[hc]`:** 32-bit MurMur3 hash by Austin Appleby.

### Memory Management and Allocators

 * **`goodalloc.[hc]`:** Simple heuristics for good buffer allocation sizes.

 * **`malloca.h`:** `MALLOCA()` and `FREEA()` macros, which are portable
   equivalents of `_malloca()` and `_freea()` from Windows SDKs by Microsoft.

 * **`memchunk.[hc]`:** Specialized allocator for situations when a lot of
   (small) allocations is needed, and which are then eventually all freed at
   once (whole "chunk"). Provides smaller overhead per-allocation than
   `malloc`, and also better data locality.

### Miscellaneous

 * **`defs.h`:** Header with miscellaneous macros such as `MIN`, `MAX`,
   `ABS`, `SIZEOF_ARRAY`, `OFFSETOF`, `CONTAINEROF`, with typical C
   implementations of those macros.

 * **`cmdline.[hc]`:** Lightweight command line (`argc`, `argv`) parsing.

### Windows-specific Modules

 * **`memstream.[hc]`:** Simple read-only memory-backed implementation of the
   COM interface `IStream`, usable to e.g. feed COM interfaces with data stored
   as Windows resources (via `LoadResourceEx()`).


## Custom Allocator Support

If macros `MALLOC_FUNC`, `REALLOC_FUNC` and/or `FREE_FUNC` are defined at the
compilation time (or in case of inline functions in headers at the time of
inclusion), then we assume those macros provide function names intended as
replacements for standard `malloc()`, `realloc()` and `free()`.

Naturally such custom functions must have the same prototype as the respective
standard functions.

Typically you may do so by using the command line option `-DMACRO=value`.
For example:

``` bash
export CFLAGS="-DMALLOC_FUNC=my_malloc -DREALLOC_FUNC=my_realloc -DFREE_FUNC=my_free"
gcc $CFLAGS myprogram.c buffer.c
```

## License

`libex` is covered with MIT license, see the file `LICENSE.md`.


## Reporting Bugs

If you encounter any bug, please be so kind and report it. Unheard bugs cannot
get fixed. You can submit bug reports here:

* http://github.com/mity/libex/issues
