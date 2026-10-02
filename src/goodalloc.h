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

#ifndef EX_GOODALLOC_H
#define EX_GOODALLOC_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Try to guess "good allocation size" which is at least the given size and
 * but hopefully friendly to the allocator to mitigate memory fragmentation.
 *
 * This should only be used when caller does not know how much exact space
 * he actually needs, e.g. for some growable buffers etc. */
size_t goodalloc_size(size_t min_size);


#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif  /* EX_GOODALLOC_H */
