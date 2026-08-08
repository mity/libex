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

#ifndef EX_STACK_H
#define EX_STACK_H

#include "buffer.h"

#ifdef __cplusplus
extern "C" {
#endif


/* This is just an inline wrapper of the simple grow-able buffer structure with
 * stack-like interface.
 */

typedef struct STACK STACK;
struct STACK { BUFFER buf; };

#define STACK_INITIALIZER           { BUFFER_INITIALIZER }

BUFFER_INLINE__ void stack_init(STACK* stack)
        { buffer_init(&stack->buf); }
BUFFER_INLINE__ void stack_fini(STACK* stack)
        { buffer_fini(&stack->buf); }

BUFFER_INLINE__ int stack_reserve(STACK* stack, size_t n)
        { return buffer_reserve(&stack->buf, n); }

BUFFER_INLINE__ size_t stack_size(const STACK* stack)
        { return buffer_size(&stack->buf); }
BUFFER_INLINE__ int stack_is_empty(const STACK* stack)
        { return buffer_is_empty(&stack->buf); }

BUFFER_INLINE__ const void* stack_const_data(const STACK* stack)
        { return buffer_const_data(&stack->buf); }
BUFFER_INLINE__ void* stack_data(STACK* stack)
        { return buffer_data(&stack->buf); }

BUFFER_INLINE__ void* stack_push_raw(STACK* stack, size_t n)
        { return buffer_append_raw(&stack->buf, n); }
BUFFER_INLINE__ int stack_push(STACK* stack, const void* data, size_t n)
        { return buffer_append(&stack->buf, data, n); }
BUFFER_INLINE__ int stack_push_int8(STACK* stack, int8_t i8)
        { return stack_push(stack, (void*) &i8, sizeof(int8_t)); }
BUFFER_INLINE__ int stack_push_uint8(STACK* stack, uint8_t u8)
        { return stack_push(stack, (void*) &u8, sizeof(uint8_t)); }
BUFFER_INLINE__ int stack_push_int16(STACK* stack, int16_t i16)
        { return stack_push(stack, (void*) &i16, sizeof(int16_t)); }
BUFFER_INLINE__ int stack_push_uint16(STACK* stack, uint16_t u16)
        { return stack_push(stack, (void*) &u16, sizeof(uint16_t)); }
BUFFER_INLINE__ int stack_push_int32(STACK* stack, int32_t i32)
        { return stack_push(stack, (void*) &i32, sizeof(int32_t)); }
BUFFER_INLINE__ int stack_push_uint32(STACK* stack, uint32_t u32)
        { return stack_push(stack, (void*) &u32, sizeof(uint32_t)); }
BUFFER_INLINE__ int stack_push_int64(STACK* stack, int64_t i64)
        { return stack_push(stack, (void*) &i64, sizeof(int64_t)); }
BUFFER_INLINE__ int stack_push_uint64(STACK* stack, uint64_t u64)
        { return stack_push(stack, (void*) &u64, sizeof(uint64_t)); }
BUFFER_INLINE__ int stack_push_ptr(STACK* stack, const void* ptr)
        { return stack_push(stack, (void*) &ptr, sizeof(void*)); }

BUFFER_INLINE__ const void* stack_peek_raw(const STACK* stack, size_t n)
        { return buffer_const_data_at(&stack->buf, stack_size(stack) - n); }
BUFFER_INLINE__ void stack_peek(const STACK* stack, void* addr, size_t n)
        { memcpy(addr, buffer_const_data_at(&stack->buf, stack_size(stack) - n), n); }
BUFFER_INLINE__ int8_t stack_peek_int8(const STACK* stack)
        { int8_t ret; stack_peek(stack, (void*) &ret, sizeof(int8_t)); return ret; }
BUFFER_INLINE__ uint8_t stack_peek_uint8(const STACK* stack)
        { uint8_t ret; stack_peek(stack, (void*) &ret, sizeof(uint8_t)); return ret; }
BUFFER_INLINE__ int16_t stack_peek_int16(const STACK* stack)
        { int16_t ret; stack_peek(stack, (void*) &ret, sizeof(int16_t)); return ret; }
BUFFER_INLINE__ uint16_t stack_peek_uint16(const STACK* stack)
        { uint16_t ret; stack_peek(stack, (void*) &ret, sizeof(uint16_t)); return ret; }
BUFFER_INLINE__ int32_t stack_peek_int32(const STACK* stack)
        { int32_t ret; stack_peek(stack, (void*) &ret, sizeof(int32_t)); return ret; }
BUFFER_INLINE__ uint32_t stack_peek_uint32(const STACK* stack)
        { uint32_t ret; stack_peek(stack, (void*) &ret, sizeof(uint32_t)); return ret; }
BUFFER_INLINE__ int64_t stack_peek_int64(const STACK* stack)
        { int64_t ret; stack_peek(stack, (void*) &ret, sizeof(int64_t)); return ret; }
BUFFER_INLINE__ uint64_t stack_peek_uint64(const STACK* stack)
        { uint64_t ret; stack_peek(stack, (void*) &ret, sizeof(uint64_t)); return ret; }
BUFFER_INLINE__ void* stack_peek_ptr(const STACK* stack)
        { void* ret; stack_peek(stack, (void*) &ret, sizeof(void*)); return ret; }

BUFFER_INLINE__ void* stack_pop_raw(STACK* stack, size_t n)
        { stack->buf.size -= n; return buffer_data_at(&stack->buf, stack->buf.size); }
BUFFER_INLINE__ void stack_pop(STACK* stack, void* addr, size_t n)
        { stack_peek(stack, addr, n); buffer_remove(&stack->buf, stack_size(stack) - n, n); }
BUFFER_INLINE__ int8_t stack_pop_int8(STACK* stack)
        { int8_t ret; stack_pop(stack, (void*) &ret, sizeof(int8_t)); return ret; }
BUFFER_INLINE__ uint8_t stack_pop_uint8(STACK* stack)
        { uint8_t ret; stack_pop(stack, (void*) &ret, sizeof(uint8_t)); return ret; }
BUFFER_INLINE__ int16_t stack_pop_int16(STACK* stack)
        { int16_t ret; stack_pop(stack, (void*) &ret, sizeof(int16_t)); return ret; }
BUFFER_INLINE__ uint16_t stack_pop_uint16(STACK* stack)
        { uint16_t ret; stack_pop(stack, (void*) &ret, sizeof(uint16_t)); return ret; }
BUFFER_INLINE__ int32_t stack_pop_int32(STACK* stack)
        { int32_t ret; stack_pop(stack, (void*) &ret, sizeof(int32_t)); return ret; }
BUFFER_INLINE__ uint32_t stack_pop_uint32(STACK* stack)
        { uint32_t ret; stack_pop(stack, (void*) &ret, sizeof(uint32_t)); return ret; }
BUFFER_INLINE__ int64_t stack_pop_int64(STACK* stack)
        { int64_t ret; stack_pop(stack, (void*) &ret, sizeof(int64_t)); return ret; }
BUFFER_INLINE__ uint64_t stack_pop_uint64(STACK* stack)
        { uint64_t ret; stack_pop(stack, (void*) &ret, sizeof(uint64_t)); return ret; }
BUFFER_INLINE__ void* stack_pop_ptr(STACK* stack)
        { void* ret; stack_pop(stack, (void*) &ret, sizeof(void*)); return ret; }

BUFFER_INLINE__ void stack_clear(STACK* stack)
        { buffer_clear(&stack->buf); }

BUFFER_INLINE__ void stack_swap(STACK* stack1, STACK* stack2)
        { buffer_swap(&stack1->buf, &stack2->buf); }


#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif  /* EX_STACK_H */
