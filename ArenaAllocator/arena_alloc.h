#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef unsigned char byte;

// States
typedef enum {
	SUCCESS,
	ERROR_MEMORY_RESERVATION
} State;

// Arena structure
struct Arena {
	byte* buff;
	int64_t size; // offset without padding
	int64_t offset;
	int64_t capacity;
};

// Api declaration
State arena_create(struct Arena* arena, int64_t size, bool initZero);
void arena_shrink(struct Arena* arena, int64_t amount); // Just shrinks the workable memory (not returning to OS)
State arena_update_buffer(struct Arena* arena, int64_t newSize, bool initZero);
void* arena_allocate(struct Arena* arena, int64_t size);
void* arena_allocate_align(struct Arena* arena, int64_t size, int64_t alignment);
void arena_flush(struct Arena* arena);
void arena_destroy(struct Arena* arena);

// Api implementation
#ifdef ARENA_ALLOC_IMPELEMENTATION

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

#ifndef ARENA_ALLOC_NO_LOG
#include <stdio.h>
#endif

#include <string.h>

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

State arena_create(struct Arena* arena, int64_t size, bool initZero)
{
	byte* buffer = mem_reserve((size_t) size, initZero);
	if (!buffer)
		return ERROR_MEMORY_RESERVATION;

	struct Arena a = {
		.buff = buffer,
		.size = 0,
		.offset = 0,
		.capacity = size
	};

	*arena = a;
}

void arena_shrink(struct Arena* arena, int64_t amount)
{
	if ((arena->capacity - amount) >= 0) {
		arena->size = 0;
		arena->offset = 0;
		arena->capacity -= amount;
	}
}

State arena_update_buffer(struct Arena* arena, int64_t newSize, bool initZero)
{
	byte* newBuffer = mem_reserve(newSize, initZero);
	if (!newBuffer)
		return ERROR_MEMORY_RESERVATION;

	mem_free(arena->buff, arena->capacity);
	arena->buff = newBuffer;
	arena->capacity = newSize;
	arena->size = 0;
	arena->offset = 0;
	return SUCCESS;
}

void* arena_allocate_align(struct Arena* arena, int64_t size, int64_t alignment)
{
	uintptr_t curr_ptr = (uintptr_t) arena->buff + (uintptr_t) arena->offset;
	uintptr_t offset = align_forward(curr_ptr, alignment);
	offset -= (uintptr_t) arena->buff; // Change to relative offset

	if (offset + size > arena->capacity) {
#ifndef ARENA_ALLOC_NO_LOG
		fprintf(stderr, "Arena out of space! [capacity => %li]\n", arena->capacity);
#endif
		return NULL;
	}

	void *ptr = arena->buff + offset;
	arena->offset = offset + size;
	arena->size += size;

	return ptr;
}

void* arena_allocate(struct Arena* arena, int64_t size)
{
	return arena_allocate_align(arena, size, DEFAULT_ALIGNMENT);
}

void arena_flush(struct Arena* arena)
{
	arena->size = 0;
	arena->offset = 0;
}

void arena_destroy(struct Arena* arena)
{
	mem_free(arena->buff, arena->capacity);
	arena->capacity = -1;
	arena->size = -1;
	arena->offset = -1;		
}

#endif // ARENA_ALLOC_IMPELEMENTATION
