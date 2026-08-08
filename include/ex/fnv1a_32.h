/*
 * libex
 * <http://github.com/mity/libex>
 *
 * Copyright (c) 2016-2026 Martin Mitáš
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

#ifndef EX_FNV1A32_H
#define EX_FNV1A32_H

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#if defined __cplusplus
    #define FNV1A32_INLINE__    inline
#elif defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
    #define FNV1A32_INLINE__    static inline
#elif defined __GNUC__
    #define FNV1A32_INLINE__    static __inline__
#elif defined _MSC_VER
    #define FNV1A32_INLINE__    static __inline
#else
    #define FNV1A32_INLINE__    static
#endif


/* 32-bit Fowler-Noll-Vo hash implementation.
 * (http://www.isthe.com/chongo/tech/comp/fnv/)
 *
 * We implement 1a variant of the function as it is generally recommended
 * and preferred over the original variant 1.
 */

#define FNV1A_32_INIT       ((uint32_t)2166136261U)

FNV1A32_INLINE__ uint32_t fnv1a_32_beg(void)
    { return FNV1A_32_INIT; }
uint32_t fnv1a_32_part(uint32_t fnv1a, const void* data, size_t n);


FNV1A32_INLINE__ uint32_t fnv1a_32(const void* data, size_t n)
    { return fnv1a_32_part(FNV1A_32_INIT, data, n); }


#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif  /* EX_FNV1A32_H */
