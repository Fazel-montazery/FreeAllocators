#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef unsigned char byte;

// Arena structure
struct Arena {
	byte* buff;
	int64_t size; // offset without padding
	int64_t offset;
	int64_t capacity;
};

// Api declaration
struct Arena arena_create(byte* buffer, int64_t size, bool initZero);
void arena_shrink(struct Arena* arena, int64_t amount); // Just shrinks the workable memory (not returning to OS)
void arena_update_buffer(struct Arena* arena, byte* newBuffer, int64_t newSize, bool initZero);
void* arena_allocate(struct Arena* arena, int64_t size);
void* arena_allocate_align(struct Arena* arena, int64_t size, int64_t alignment);
void arena_flush(struct Arena* arena);

// Api implementation
#ifdef ARENA_ALLOC_IMPELEMENTATION

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

#ifndef ARENA_ALLOC_NO_LOG
#include <stdio.h>
#endif

#include <string.h>

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

struct Arena arena_create(byte* buffer, int64_t size, bool initZero)
{
	if (initZero) memset(buffer, 0, size);
	return (struct Arena) {
		.buff = buffer,
		.size = 0,
		.offset = 0,
		.capacity = size
	};
}

void arena_shrink(struct Arena* arena, int64_t amount)
{
	if ((arena->capacity - amount) >= 0) {
		arena->size = 0;
		arena->offset = 0;
		arena->capacity -= amount;
	}
}

void arena_update_buffer(struct Arena* arena, byte* newBuffer, int64_t newSize, bool initZero)
{
	if (initZero) memset(newBuffer, 0, newSize);
	arena->buff = newBuffer;
	arena->capacity = newSize;
	arena->size = 0;
	arena->offset = 0;
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

#endif // ARENA_ALLOC_IMPELEMENTATION
