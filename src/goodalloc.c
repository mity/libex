/*
 * libex
 * <http://github.com/mity/libex>
 *
 * Copyright (c) 2018-2026 Martin Mitáš
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <limits.h>
#include <stdint.h>
#include "goodalloc.h"


#if defined __APPLE__ && defined(__has_include)
    #if __has_include(<malloc/malloc.h>)
        #include <malloc/malloc.h>
        #define GOODALLOC_SYS_FUNC      malloc_good_size
    #endif
#endif


static size_t
goodalloc_power_of_two(size_t v)
{
    size_t shift;

    /* Based on https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
     * except we rely on the compiler to unroll the loop for us; this is better
     * as we don't need to hardcode any assumptions about how many bits size_t
     * actually has. */
    v--;
    for(shift = 1; shift < sizeof(size_t) * CHAR_BIT; shift <<= 1)
        v |= v >> shift;
    v++;

    return v;
}

/* In approximation, we round up to the nearest larger power of two. This is to
 * guarantee amortized O(1) complexity for growing reallocations.
 *
 * For bigger allocations we subtract some (hopefully right) number of bytes
 * for malloc's internal overhead in order to mitigate memory fragmentation
 * when goodalloc_size() is used heavily to guide its (re)allocations.
 *
 * The idea is that (except for allocations up to GOODALLOC_SMALLMAXSIZE) any
 * two smaller allocations ought to fit into a memory window of twice as large
 * freed block (including their overhead).
 */

#define GOODALLOC_SMALLMINSIZE      16
#define GOODALLOC_SMALLMAXSIZE      256
#define GOODALLOC_OVERHEADSIZE      ((sizeof(void*) > 4) ? 16 : 8)

size_t
goodalloc_size(size_t min_size)
{
    size_t size = min_size;

    if(size < GOODALLOC_SMALLMINSIZE) {
        size = GOODALLOC_SMALLMINSIZE;
    } else if(size <= GOODALLOC_SMALLMAXSIZE) {
        size = goodalloc_power_of_two(size);
    } else if(size > SIZE_MAX / 2 - GOODALLOC_OVERHEADSIZE) {
        /* Prevention of any possibility of an overflow in the code below. */
        size = SIZE_MAX;
    } else {
        size += GOODALLOC_OVERHEADSIZE;
        size = goodalloc_power_of_two(size);
        size -= GOODALLOC_OVERHEADSIZE;
    }

#ifdef GOODALLOC_SYS_FUNC
    size = GOODALLOC_SYS_FUNC(size);
#endif

    return size;
}
