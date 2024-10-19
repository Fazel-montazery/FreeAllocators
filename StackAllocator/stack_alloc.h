#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef unsigned char byte;

// States
typedef enum {
	SUCCESS,
	ERROR_MEMORY_RESERVATION
} State;

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
	int64_t extra;
};

// Api declaration
State stack_create(struct Stack* stack, int64_t size, bool initZero);
State stack_create_extra(struct Stack* stack, int64_t size, int64_t extra, bool initZero);
void stack_shrink(struct Stack* stack, int64_t amount);
void stack_updtae_buffer(struct Stack* stack, byte* newBuffer, int64_t newSize, bool initZero);
void* stack_allocate(struct Stack* stack, int64_t size);
void* stack_allocate_align(struct Stack* stack, int64_t size, int64_t alignment);
void stack_pop(struct Stack* stack);
void stack_flush(struct Stack* stack);
void stack_destroy(struct Stack* stack);

// Api implementation
#ifdef STACK_ALLOC_IMPELEMENTATION

#ifndef STACK_ALLOC_NO_LOG
#include <stdio.h>
#endif

#include <string.h>

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

#ifndef EXTRA_CAP
#define EXTRA_CAP 1024 // for extra padding
#endif

#ifndef HEADER_SIZE
#define HEADER_SIZE (sizeof(_header_t))
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

static void* mem_reserve(size_t size, bool zeroOut)
{
#ifdef _WIN32
	if (zeroOut)
		return VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	else
		return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
#else
	void* ptr;
	#if defined __APPLE__ && defined __MACH__
        int flags = MAP_PRIVATE | MAP_ANON;
    	#else
        int flags = MAP_PRIVATE | MAP_ANONYMOUS;
    	#endif

	if((ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, flags, -1, 0)) == MAP_FAILED)
		return NULL;

	return ptr;
#endif
}

static void mem_free(void* ptr, size_t size)
{
#ifdef _WIN32
	VirtualFree(ptr, 0, MEM_RELEASE);
#else
	munmap(ptr, size);
#endif
}

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

State stack_create_extra(struct Stack* stack, int64_t size, int64_t extra, bool initZero)
{
	byte* buffer = mem_reserve((size_t) size + (size_t) extra, initZero);
	if (!buffer)
		return ERROR_MEMORY_RESERVATION;

	struct Stack s = {
		.buff = buffer,
		.currHeader = NULL,
		.size = 0,
		.offset = 0,
		.capacity = size + extra,
		.extra = extra
	};

	*stack = s;
	return SUCCESS;
}

State stack_create(struct Stack* stack, int64_t size, bool initZero)
{
	return stack_create_extra(stack, size, EXTRA_CAP, initZero);
}

void stack_shrink(struct Stack* stack, int64_t amount)
{
	if (((stack->capacity - stack->extra) - amount) >= 0) {
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

void* stack_allocate_align(struct Stack* stack, int64_t size, int64_t alignment)
{
	uintptr_t curr_ptr = (uintptr_t) stack->buff + (uintptr_t) stack->offset + (uintptr_t) HEADER_SIZE;
	uintptr_t offset = align_forward(curr_ptr, DEFAULT_ALIGNMENT);
	
	offset -= (uintptr_t) stack->buff; // Change to relative offset
					   
	if ((offset + size) > (stack->capacity - stack->extra)) {
#ifndef STACK_ALLOC_NO_LOG
		fprintf(stderr, "Stack out of space! [capacity => %li]\n", stack->capacity);
#endif
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

void* stack_allocate(struct Stack* stack, int64_t size) 
{
	return stack_allocate_align(stack, size, DEFAULT_ALIGNMENT);
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

void stack_destroy(struct Stack* stack)
{
	mem_free(stack->buff, stack->capacity);
	stack->capacity = -1;
	stack->size = -1;
	stack->offset = -1;
	stack->extra = -1;
	stack->currHeader = NULL;
}

#endif // STACK_ALLOC_IMPELEMENTATION
