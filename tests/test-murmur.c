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

#include "acutest.h"
#include "murmur3_32.h"


typedef struct TEST_VECTOR {
    const char* str;
    size_t n;
    uint32_t seed;
    uint32_t hash;
} TEST_VECTOR;

#define LEN(x)      (sizeof(x)-1)
#define TEST(x)     x, LEN(x)


/* from https://stackoverflow.com/questions/14747343/murmurhash3-test-vectors */
static const TEST_VECTOR test_vectrors[] = {
    { TEST(""),                          0U, 0x00000000U },
    { TEST(""),                          1U, 0x514e28b7U },
    { TEST(""),                 0xffffffffU, 0x81f16f39U },
    { TEST("\xff\xff\xff\xff"),          0U, 0x76293b50U },
    { TEST("\x21\x43\x65\x87"),          0U, 0xf55b516bU },
    { TEST("\x21\x43\x65\x87"), 0x5082edeeU, 0x2362f9deU },
    { TEST("\x21\x43\x65"),              0U, 0x7e4a8634U },
    { TEST("\x21\x43"),                  0U, 0xa0f7b07aU },
    { TEST("\x21"),                      0U, 0x72661cf4U },
    { TEST("\x00\x00\x00\x00"),          0U, 0x2362f9deU },
    { TEST("\x00\x00\x00"),              0U, 0x85f0b427U },
    { TEST("\x00\x00"),                  0U, 0x30f4c306U },
    { TEST("\x00"),                      0U, 0x514e28b7U },
    { 0 }
};


static void
test_murmur3_32(void)
{
    int i;

    for(i = 0; test_vectrors[i].str != NULL; i++) {
        const char* str = test_vectrors[i].str;
        size_t n = test_vectrors[i].n;
        uint32_t expected = test_vectrors[i].hash;
        uint32_t produced;

        produced = murmur3_32(test_vectrors[i].seed, str, n);
        if(!TEST_CHECK_(produced == expected, "vector '%.*s'", (int)n, str)) {
            TEST_MSG("Expected: %x", (unsigned) expected);
            TEST_MSG("Produced: %x", (unsigned) produced);
        }
    }
}

static void
x(void)
{
    uint32_t ha = 0;
    uint32_t hb = 0;

    ha = murmur3_32(ha, "hello", 5);

    hb = murmur3_32(hb, "hel", 3);
    hb = murmur3_32(hb, "lo", 2);

    if(!TEST_CHECK(ha == hb)) {
        TEST_MSG("ha: %x", (unsigned) ha);
        TEST_MSG("hb: %x", (unsigned) hb);
    }
}


TEST_LIST = {
    { "x"              , x },
    { "test-murmur3-32", test_murmur3_32 },
    { 0 }
};
