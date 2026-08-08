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

#ifndef EX_BUFFER_H
#define EX_BUFFER_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


#if defined __cplusplus
    #define BUFFER_INLINE__     inline
#elif defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
    #define BUFFER_INLINE__     static inline
#elif defined __GNUC__
    #define BUFFER_INLINE__     static __inline__
#elif defined _MSC_VER
    #define BUFFER_INLINE__     static __inline
#else
    #define BUFFER_INLINE__     static
#endif


/* Simple automatically growing buffer implementation.
 * The realloc() growth is exponential to provide constant amortized time
 * complexity O(1).
 */


typedef struct BUFFER {
    void* data;
    size_t size;
    size_t alloc;
} BUFFER;


/* Static initializer. */
#define BUFFER_INITIALIZER          { NULL, 0, 0 }

/* Initialize/deinitialize buffer structure. */
BUFFER_INLINE__ void buffer_init(BUFFER* buf)
        { buf->data = NULL; buf->size = 0; buf->alloc = 0; }
BUFFER_INLINE__ void buffer_fini(BUFFER* buf)
        { free(buf->data); }

/* Change capacity of the buffer.
 *
 * This may be useful e.g. after initialization if the caller has an idea how
 * much data shall be inserted into the buffer to avoid some reallocations
 * during subsequent insertions.
 *
 * buffer_realloc() reallocates the buffer to the given exact (absolute) size.
 * If lower than current size, the buffer is truncated.
 *
 * buffer_reserve() makes sure there is at least N free bytes in the buffer
 * on top of already used capacity.
 */
int buffer_realloc(BUFFER* buf, size_t alloc);
int buffer_reserve(BUFFER* buf, size_t n);

/* Shrink the buffer not to hold an excessive amount of unused memory. */
void buffer_shrink(BUFFER* buf);

BUFFER_INLINE__ size_t buffer_size(const BUFFER* buf)
        { return buf->size; }
BUFFER_INLINE__ int buffer_is_empty(const BUFFER* buf)
        { return (buf->size == 0); }

/* Const accessors. */
BUFFER_INLINE__ const void* buffer_const_data(const BUFFER* buf)
        { return buf->data; }
BUFFER_INLINE__ const void* buffer_const_data_at(const BUFFER* buf, size_t off)
        { return (const void*) (((const uint8_t*)buf->data) + off); }

/* Mutable accessors. */
BUFFER_INLINE__ void* buffer_data(BUFFER* buf)
        { return buf->data; }
BUFFER_INLINE__ void* buffer_data_at(BUFFER* buf, size_t off)
        { return (void*) (((uint8_t*)buf->data) + off); }

/* Inserting N bytes.
 * The _raw variant on success returns pointer where app is supposed to write
 * N bytes; or NULL on error. */
void* buffer_insert_raw(BUFFER* buf, size_t off, size_t n);
int buffer_insert(BUFFER* buf, size_t off, const void* data, size_t n);

/* Appending.
 * The _raw variant on success returns pointer where app is supposed to write
 * N bytes; or NULL on error. */
BUFFER_INLINE__ void* buffer_append_raw(BUFFER* buf, size_t n)
        { return buffer_insert_raw(buf, buf->size, n); }
BUFFER_INLINE__ int buffer_append(BUFFER* buf, const void* data, size_t n)
        { return buffer_insert(buf, buf->size, data, n); }

/* Remove N bytes from the given offset. */
void buffer_remove(BUFFER* buf, size_t off, size_t n);

/* Remove all buffer contents. */
BUFFER_INLINE__ void buffer_clear(BUFFER* buf)
        { buffer_remove(buf, 0, buf->size); }

/* Take over the responsibility of the buffer contents. Caller then
 * eventually must free() the returned block. */
BUFFER_INLINE__ void* buffer_acquire(BUFFER* buf, size_t* p_size)
        { void* data = buf->data; if(p_size != NULL) *p_size = buf->size;
          buffer_init(buf); return data; }

/* Swap contents of two buffers. */
BUFFER_INLINE__ void buffer_swap(BUFFER* buf1, BUFFER* buf2)
        { BUFFER tmp; tmp = *buf1; *buf1 = *buf2; *buf2 = tmp; }


#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif  /* EX_BUFFER_H */
