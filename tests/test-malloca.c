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



#include "acutest.h"

/* We use our own allocator functions for testing purposes. */
#define MALLOC_FUNC     my_malloc
#define FREE_FUNC       my_free
#include "malloca.h"

#include <stdint.h>


static unsigned malloc_counter = 0;
static unsigned free_counter = 0;

void*
my_malloc(size_t size)
{
    malloc_counter++;
    return malloc(size);
}

void
my_free(void* ptr)
{
    free_counter++;
    free(ptr);
}


static void
test_malloca_zero(void)
{
    /* MALLOCA with zero size should not return NULL, but something unique
     * what can be passed to FREEA(). */
    void* ptr;

    ptr = MALLOCA(0);
    TEST_CHECK(ptr != NULL);
    FREEA(ptr);
}

static void
test_malloca_small(void)
{
    /* Test that small allocations are on stack and on heap; i.e. that our
     * my_malloc() and my_free() are never called. */
    unsigned old_malloc_counter;
    unsigned old_free_counter;
    void* ptr;

    old_malloc_counter = malloc_counter;
    old_free_counter = free_counter;

    ptr = MALLOCA(20);
    TEST_ASSERT(ptr != NULL);
    strcpy(ptr, "hello");
    FREEA(ptr);

    TEST_CHECK(malloc_counter == old_malloc_counter);
    TEST_CHECK(free_counter == old_free_counter);
}

static void
test_malloca_large(void)
{
    /* In contrast large MALLOCA allocations should be on heap. */
    unsigned old_malloc_counter;
    unsigned old_free_counter;
    void* ptr;

    old_malloc_counter = malloc_counter;
    old_free_counter = free_counter;

    ptr = MALLOCA(16 * 1024);
    TEST_ASSERT(ptr != NULL);
    strcpy(ptr, "hello");
    FREEA(ptr);

    TEST_CHECK(malloc_counter == old_malloc_counter + 1);
    TEST_CHECK(free_counter == old_free_counter + 1);
}

static void
test_malloca_ultralarge(void)
{
    /* Attempt to allocate something ultra-large should always fail and return
     * NULL, the same way as malloc() does.
     *
     * The ridiculous expressions are to pass over warning
     * -Walloc-size-larger-than= included in -Wall (gcc 7.2.0 on Linux).
     */
    size_t size = SIZE_MAX / 2 - 8;
    void* ptr;


    /* Verify that malloc() fails. If not, we have chosen too small size
     * and our test needs some tuning... */
    ptr = malloc(size);
    TEST_CHECK(ptr == NULL);
    free(ptr);      /* Should not be needed if it really works ;-) */

    /* And now test that MALLOCA fails the same way. */
    ptr = MALLOCA(size);
    TEST_CHECK(ptr == NULL);
    free(ptr);      /* Should not be needed if it really works ;-) */
}

static void
test_freea_null(void)
{
    /* Just check this does not cause SIGSEGV. */

    FREEA(NULL);
    TEST_CHECK_(1, "FREEA(NULL) is noop that never crashes.");
}


TEST_LIST = {
    { "malloca-zero",       test_malloca_zero },
    { "malloca-small",      test_malloca_small },
    { "malloca-large",      test_malloca_large },
    { "malloca-ultralarge", test_malloca_ultralarge },
    { "freea-null",         test_freea_null },
    { 0 }
};

