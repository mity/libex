/*
 * libex
 * <http://github.com/mity/libex>
 *
 * Copyright (c) 2026 Martin Mitáš
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

/*
 * Original MurmurHash3 [1] was written by Austin Appleby, and was placed in
 * the public domain.
 *
 * Copyright note and license terms above cover only the modifications and
 * additions made by the above copyright holder(s). The original public-domain
 * portions of this file remain in the public domain.
 *
 * [1]: https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp
 */

#include "murmur3_32.h"

#if defined __GNUC__ || defined(__clang__)
    #define INLINE__            __inline__ __attribute__((always_inline))
#elif defined _MSC_VER
    #define INLINE__            __forceinline
#elif defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
    #define INLINE__            inline
#else
    #define INLINE__
#endif


#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 202311L
    #define FALLTHOUGH          [[fallthrough]]
#elif defined __GNUC__ || defined __clang__
    #define FALLTHOUGH          [[fallthrough]]
#else
    #define FALLTHOUGH          do {} while(0)
#endif


#if defined(_MSC_VER)
    #include <stdlib.h>
    #define ROTL32(x,y)	        _rotl(x, y)
#elif defined(__has_builtin) && __has_builtin(__builtin_stdc_rotate_left)
    #define ROTL32(x,y)         __builtin_stdc_rotate_left(x, y)
#else
    #define ROTL32(x,y)         (((x) << (y)) | ((x) >> (32 - (y))))
#endif


/* We have faster implementation path for little-endian systems; especially
 * if they allow (fast) unaligned memory access like Intel. */
#if defined __has_include && __has_include(<sys/param.h>)
    /* On some systems, this provides __BYTE_ORDER__ macro. */
    #include <sys/param.h>
#endif
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ||                              \
    BYTE_ORDER == LITTLE_ENDIAN ||                                            \
    defined __LITTLE_ENDIAN__ || defined LITTLE_ENDIAN ||                     \
    defined __AARCH64EL__ ||                                                  \
    defined __ARMEL__ ||                                                      \
    defined i386 || defined __i386 || defined __i386__ ||                     \
    defined _MIPSEL || defined __MIPSEL || defined __MIPSEL__ ||              \
    defined __THUMBEL__
    #define HAS_LE_BYTEORDER                1
#endif
#if defined i386 || defined __i386 || defined __i386__
    #define HAS_FAST_UNALIGNED_MEMACCESS    1
#endif


#define C1      0xcc9e2d51U
#define C2      0x1b873593U
#define C3      0xe6546b64U


#if !HAS_LE_BYTEORDER || !HAS_FAST_UNALIGNED_MEMACCESS
static inline uint32_t
murmur3_32_body_generic(uint32_t h1, const uint8_t* body, size_t len)
{
    uint32_t k1;
    size_t i;

    for(i = 0; i < len; i += 4) {
        k1 = (body[i+0] <<  0) | (body[i+1] <<  8) |
             (body[i+2] << 16) | (body[i+3] << 24);

        k1 *= C1;
        k1 = ROTL32(k1, 15);
        k1 *= C2;

        h1 ^= k1;
        h1 = ROTL32(h1, 13);
        h1 = h1 * 5 + C3;
    }

    return h1;
}
#endif

#if HAS_LE_BYTEORDER
static inline uint32_t
murmur3_32_body_by_blocks(uint32_t h1, const uint8_t* body, size_t len)
{
    const uint32_t* blocks = (const uint32_t*)body;
    size_t n_blocks = len / 4;
    uint32_t k1;
    size_t i;

    for(i = 0; i < n_blocks; i++) {
        k1 = blocks[i];

        k1 *= C1;
        k1 = ROTL32(k1, 15);
        k1 *= C2;

        h1 ^= k1;
        h1 = ROTL32(h1, 13);
        h1 = h1 * 5 + C3;
    }

    return h1;
}
#endif

uint32_t
murmur3_32(uint32_t seed, const void* data, size_t len)
{
    size_t tail_len = (len & 3);
    size_t body_len = len - tail_len;
    const uint8_t* body = (const uint8_t*)data;
    const uint8_t* tail = body + body_len;
    uint32_t h1 = seed;
    uint32_t k1;

    /* Body */
#if HAS_LE_BYTEORDER  &&  HAS_FAST_UNALIGNED_MEMACCESS
    h1 = murmur3_32_body_by_blocks(h1, body, body_len);
#elif HAS_LE_BYTEORDER
    if(((uintptr_t)body & 3) == 0)
        h1 = murmur3_32_body_by_blocks(h1, body, body_len);
    else
        h1 = murmur3_32_body_generic(h1, body, body_len);
#else
    h1 = murmur3_32_body_generic(h1, body, body_len);
#endif

    /* Tail */
    k1 = 0;
    switch(tail_len) {
        case 3:
            k1 |= tail[2] << 16;
            FALLTHOUGH;

        case 2:
            k1 |= tail[1] << 8;
            FALLTHOUGH;

        case 1:
            k1 |= tail[0];
            k1 *= C1;
            k1 = ROTL32(k1, 15);
            k1 *= C2;
            h1 ^= k1;
    };

    /* Finalization */
    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return h1;
}
