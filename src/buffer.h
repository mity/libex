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


/* Simple automatically growing buffer implementation.
 * The realloc() growth is exponential to provide constant amortized time
 * complexity O(1).
 */


typedef struct Buffer {
    void* data;
    size_t size;
    size_t alloc;
} Buffer;


/* Static initializer. */
#define BUFFER_INITIALIZER          { NULL, 0, 0 }

/* Initialize/deinitialize buffer structure. */
void buffer_init(Buffer* buf);
void buffer_fini(Buffer* buf);

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
int buffer_realloc(Buffer* buf, size_t alloc);
int buffer_reserve(Buffer* buf, size_t n);

/* Shrink the buffer not to hold an excessive amount of unused memory. */
void buffer_shrink(Buffer* buf);

static inline size_t buffer_size(const Buffer* buf)
        { return buf->size; }
static inline int buffer_is_empty(const Buffer* buf)
        { return (buf->size == 0); }

/* Const accessors. */
static inline const void* buffer_const_data(const Buffer* buf)
        { return buf->data; }
static inline const void* buffer_const_data_at(const Buffer* buf, size_t off)
        { return (const void*) (((const uint8_t*)buf->data) + off); }

/* Mutable accessors. */
static inline void* buffer_data(Buffer* buf)
        { return buf->data; }
static inline void* buffer_data_at(Buffer* buf, size_t off)
        { return (void*) (((uint8_t*)buf->data) + off); }

/* Inserting N bytes.
 * The _raw variant on success returns pointer where app is supposed to write
 * N bytes; or NULL on error. */
void* buffer_insert_raw(Buffer* buf, size_t off, size_t n);
int buffer_insert(Buffer* buf, size_t off, const void* data, size_t n);

/* Appending.
 * The _raw variant on success returns pointer where app is supposed to write
 * N bytes; or NULL on error. */
static inline void* buffer_append_raw(Buffer* buf, size_t n)
        { return buffer_insert_raw(buf, buf->size, n); }
static inline int buffer_append(Buffer* buf, const void* data, size_t n)
        { return buffer_insert(buf, buf->size, data, n); }

/* Remove N bytes from the given offset. */
void buffer_remove(Buffer* buf, size_t off, size_t n);

/* Remove all buffer contents. */
static inline void buffer_clear(Buffer* buf)
        { buffer_remove(buf, 0, buf->size); }

/* Take over the responsibility of the buffer contents. Caller then
 * eventually must free() the returned block. */
static inline void* buffer_acquire(Buffer* buf, size_t* p_size)
        { void* data = buf->data; if(p_size != NULL) *p_size = buf->size;
          buffer_init(buf); return data; }

/* Swap contents of two buffers. */
static inline void buffer_swap(Buffer* buf1, Buffer* buf2)
        { Buffer tmp; tmp = *buf1; *buf1 = *buf2; *buf2 = tmp; }


#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif  /* EX_BUFFER_H */
