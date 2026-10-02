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

typedef struct Stack Stack;
struct Stack { Buffer buf; };

#define STACK_INITIALIZER           { BUFFER_INITIALIZER }

static inline void stack_init(Stack* stack)
        { buffer_init(&stack->buf); }
static inline void stack_fini(Stack* stack)
        { buffer_fini(&stack->buf); }

static inline int stack_reserve(Stack* stack, size_t n)
        { return buffer_reserve(&stack->buf, n); }

static inline size_t stack_size(const Stack* stack)
        { return buffer_size(&stack->buf); }
static inline int stack_is_empty(const Stack* stack)
        { return buffer_is_empty(&stack->buf); }

static inline const void* stack_const_data(const Stack* stack)
        { return buffer_const_data(&stack->buf); }
static inline void* stack_data(Stack* stack)
        { return buffer_data(&stack->buf); }

static inline void* stack_push_raw(Stack* stack, size_t n)
        { return buffer_append_raw(&stack->buf, n); }
static inline int stack_push(Stack* stack, const void* data, size_t n)
        { return buffer_append(&stack->buf, data, n); }
static inline int stack_push_int8(Stack* stack, int8_t i8)
        { return stack_push(stack, (void*) &i8, sizeof(int8_t)); }
static inline int stack_push_uint8(Stack* stack, uint8_t u8)
        { return stack_push(stack, (void*) &u8, sizeof(uint8_t)); }
static inline int stack_push_int16(Stack* stack, int16_t i16)
        { return stack_push(stack, (void*) &i16, sizeof(int16_t)); }
static inline int stack_push_uint16(Stack* stack, uint16_t u16)
        { return stack_push(stack, (void*) &u16, sizeof(uint16_t)); }
static inline int stack_push_int32(Stack* stack, int32_t i32)
        { return stack_push(stack, (void*) &i32, sizeof(int32_t)); }
static inline int stack_push_uint32(Stack* stack, uint32_t u32)
        { return stack_push(stack, (void*) &u32, sizeof(uint32_t)); }
static inline int stack_push_int64(Stack* stack, int64_t i64)
        { return stack_push(stack, (void*) &i64, sizeof(int64_t)); }
static inline int stack_push_uint64(Stack* stack, uint64_t u64)
        { return stack_push(stack, (void*) &u64, sizeof(uint64_t)); }
static inline int stack_push_ptr(Stack* stack, const void* ptr)
        { return stack_push(stack, (void*) &ptr, sizeof(void*)); }

static inline const void* stack_peek_raw(const Stack* stack, size_t n)
        { return buffer_const_data_at(&stack->buf, stack_size(stack) - n); }
static inline void stack_peek(const Stack* stack, void* addr, size_t n)
        { memcpy(addr, buffer_const_data_at(&stack->buf, stack_size(stack) - n), n); }
static inline int8_t stack_peek_int8(const Stack* stack)
        { int8_t ret; stack_peek(stack, (void*) &ret, sizeof(int8_t)); return ret; }
static inline uint8_t stack_peek_uint8(const Stack* stack)
        { uint8_t ret; stack_peek(stack, (void*) &ret, sizeof(uint8_t)); return ret; }
static inline int16_t stack_peek_int16(const Stack* stack)
        { int16_t ret; stack_peek(stack, (void*) &ret, sizeof(int16_t)); return ret; }
static inline uint16_t stack_peek_uint16(const Stack* stack)
        { uint16_t ret; stack_peek(stack, (void*) &ret, sizeof(uint16_t)); return ret; }
static inline int32_t stack_peek_int32(const Stack* stack)
        { int32_t ret; stack_peek(stack, (void*) &ret, sizeof(int32_t)); return ret; }
static inline uint32_t stack_peek_uint32(const Stack* stack)
        { uint32_t ret; stack_peek(stack, (void*) &ret, sizeof(uint32_t)); return ret; }
static inline int64_t stack_peek_int64(const Stack* stack)
        { int64_t ret; stack_peek(stack, (void*) &ret, sizeof(int64_t)); return ret; }
static inline uint64_t stack_peek_uint64(const Stack* stack)
        { uint64_t ret; stack_peek(stack, (void*) &ret, sizeof(uint64_t)); return ret; }
static inline void* stack_peek_ptr(const Stack* stack)
        { void* ret; stack_peek(stack, (void*) &ret, sizeof(void*)); return ret; }

static inline void* stack_pop_raw(Stack* stack, size_t n)
        { stack->buf.size -= n; return buffer_data_at(&stack->buf, stack->buf.size); }
static inline void stack_pop(Stack* stack, void* addr, size_t n)
        { stack_peek(stack, addr, n); buffer_remove(&stack->buf, stack_size(stack) - n, n); }
static inline int8_t stack_pop_int8(Stack* stack)
        { int8_t ret; stack_pop(stack, (void*) &ret, sizeof(int8_t)); return ret; }
static inline uint8_t stack_pop_uint8(Stack* stack)
        { uint8_t ret; stack_pop(stack, (void*) &ret, sizeof(uint8_t)); return ret; }
static inline int16_t stack_pop_int16(Stack* stack)
        { int16_t ret; stack_pop(stack, (void*) &ret, sizeof(int16_t)); return ret; }
static inline uint16_t stack_pop_uint16(Stack* stack)
        { uint16_t ret; stack_pop(stack, (void*) &ret, sizeof(uint16_t)); return ret; }
static inline int32_t stack_pop_int32(Stack* stack)
        { int32_t ret; stack_pop(stack, (void*) &ret, sizeof(int32_t)); return ret; }
static inline uint32_t stack_pop_uint32(Stack* stack)
        { uint32_t ret; stack_pop(stack, (void*) &ret, sizeof(uint32_t)); return ret; }
static inline int64_t stack_pop_int64(Stack* stack)
        { int64_t ret; stack_pop(stack, (void*) &ret, sizeof(int64_t)); return ret; }
static inline uint64_t stack_pop_uint64(Stack* stack)
        { uint64_t ret; stack_pop(stack, (void*) &ret, sizeof(uint64_t)); return ret; }
static inline void* stack_pop_ptr(Stack* stack)
        { void* ret; stack_pop(stack, (void*) &ret, sizeof(void*)); return ret; }

static inline void stack_clear(Stack* stack)
        { buffer_clear(&stack->buf); }

static inline void stack_swap(Stack* stack1, Stack* stack2)
        { buffer_swap(&stack1->buf, &stack2->buf); }


#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif  /* EX_STACK_H */
