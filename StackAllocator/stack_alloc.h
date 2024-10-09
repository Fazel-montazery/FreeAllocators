#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef unsigned char byte;

// Stack Header structure
typedef struct HEADER_STRUCT {
	int32_t size;
	struct HEADER_STRUCT* pre;
} _header_t;

// Stack structure
struct Stack {
	byte* buff;
	_header_t* currHeader;
	int64_t size; // offset without padding and headers
	int64_t offset;
	int64_t capacity;
};

// Api declaration
struct Stack stack_create(byte* buffer, int64_t size, bool initZero);
void stack_shrink(struct Stack* stack, int64_t amount);
void stack_updtae_buffer(struct Stack* stack, byte* newBuffer, int64_t newSize, bool initZero);
void* stack_allocate(struct Stack* stack, int64_t size);
void stack_pop(struct Stack* stack);
void stack_flush(struct Stack* stack);

// Api implementation
#ifdef STACK_ALLOC_IMPELEMENTATION

#include <stdio.h>
#include <string.h>

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

#ifndef HEADER_SIZE
#define HEADER_SIZE (sizeof(_header_t))
#endif

static uintptr_t align_forward(uintptr_t ptr, int64_t align) 
{
	uintptr_t p, a, modulo;

	p = ptr;
	a = (uintptr_t) align;
	modulo = p & (a - 1);

	if (modulo != 0) {
		p += a - modulo;
	}

	return p;
}

struct Stack stack_create(byte* buffer, int64_t size, bool initZero)
{
	if (initZero) memset(buffer, 0, size);
	return (struct Stack) {
		.buff = buffer,
		.currHeader = NULL,
		.size = 0,
		.offset = 0,
		.capacity = size
	};
}

void stack_shrink(struct Stack* stack, int64_t amount)
{
	if ((stack->capacity - amount) >= 0) {
		stack_flush(stack);
		stack->capacity -= amount;
	}
}

void stack_updtae_buffer(struct Stack* stack, byte* newBuffer, int64_t newSize, bool initZero)
{
	if (initZero) memset(newBuffer, 0, newSize);
	stack_flush(stack);
	stack->buff = newBuffer;
	stack->capacity = newSize;
}

void* stack_allocate(struct Stack* stack, int64_t size)
{
	uintptr_t curr_ptr = (uintptr_t) stack->buff + (uintptr_t) stack->offset + (uintptr_t) HEADER_SIZE;
	uintptr_t offset = align_forward(curr_ptr, DEFAULT_ALIGNMENT);
	
	offset -= (uintptr_t) stack->buff; // Change to relative offset
					   
	if ((offset + size) > stack->capacity) {
		fprintf(stderr, "Stack out of space! [capacity => %li]\n", stack->capacity);
		return NULL;
	}

	if (stack->offset == 0) {
		*((_header_t*) (curr_ptr - HEADER_SIZE)) = (_header_t) {.size = size, .pre = NULL};
	} else {
		*((_header_t*) (curr_ptr - HEADER_SIZE)) = (_header_t) {.size = size, .pre = stack->currHeader};
	}

	stack->currHeader = (_header_t*) (curr_ptr - HEADER_SIZE);

	void* ptr = stack->buff + offset;

	stack->size += size;
	stack->offset = offset + size;

	return ptr;
}

void stack_pop(struct Stack* stack)
{
	if (!stack->currHeader)
		return;

	_header_t head = *(stack->currHeader);
	stack->size -= head.size;
	stack->offset = (int64_t) stack->currHeader - (int64_t) stack->buff;
	stack->currHeader = head.pre;
}

void stack_flush(struct Stack* stack)
{
	stack->currHeader = NULL;
	stack->offset = 0;
	stack->size = 0;
}

#endif // STACK_ALLOC_IMPELEMENTATION
