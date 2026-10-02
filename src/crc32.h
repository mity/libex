/*
 * libex
 * <http://github.com/mity/libex>
 *
 * Copyright (c) 2017-2026 Martin Mitáš
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

#ifndef EX_CRC32_H
#define EX_CRC32_H

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Compute CRC-32 of the given data.
 *
 * Note there is no real CRC-32 standard and the ecosystem is quite messy
 * (see e.g. https://zlib.net/crc_v3.txt).
 *
 * This implementation is based on the appendix A.3 of the paper
 * http://stigge.org/martin/pub/SAR-PR-2006-05.pdf
 *
 * This variant is sometimes also referred to as CRC-32/ISO-HDLC, CRC-32/ADCCP,
 * CRC-32/V-42, CRC-32/XZ, or PKZIP's CRC32.
 */

/* Use these for hashing per-partes */
static inline uint32_t crc32_beg(void)
    { return 0xffffffffU; }
uint32_t crc32_part(uint32_t crc, const void* data, size_t n);
static inline uint32_t crc32_end(uint32_t crc)
    { return crc ^ 0xffffffffU; }


static inline uint32_t crc32(const void* data, size_t n)
    { uint32_t crc; crc = crc32_part(0xffffffffU, data, n); return crc ^ 0xffffffffU; }


#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif  /* EX_CRC32_H */
