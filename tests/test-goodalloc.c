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

#include <stdint.h>

#include "acutest.h"
#include "goodalloc.h"


static void
test_goodalloc(void)
{
    TEST_CHECK(goodalloc_size(0) >= 0);
    TEST_CHECK(goodalloc_size(7) >= 7);
    TEST_CHECK(goodalloc_size(8) >= 8);
    TEST_CHECK(goodalloc_size(128) >= 128);
    TEST_CHECK(goodalloc_size(SIZE_MAX/2) >= SIZE_MAX/2);
    TEST_CHECK(goodalloc_size(SIZE_MAX) >= SIZE_MAX);
}


TEST_LIST = {
    { "goodalloc", test_goodalloc },
    { 0 }
};
