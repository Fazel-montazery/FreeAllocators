#pragma once

#include <stddef.h>

typedef unsigned char byte;

// Arena structure
struct Arena {
	byte* buff;
	size_t offset;
	size_t capacity;
};

// Api declaration
struct Arena arena_create(byte* buffer, size_t size);
void* arena_allocate(struct Arena* arena, size_t size);
void arena_flush(struct Arena* arena);

// Api implementation
#ifdef ARENA_ALLOC_IMPELEMENTATION

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

static uintptr_t align_forward(uintptr_t ptr, size_t align) 
{
	uintptr_t p, a, modulo;

	p = ptr;
	a = (uintptr_t)align;
	modulo = p & (a - 1);

	if (modulo != 0) {
		p += a - modulo;
	}

	return p;
}

static void* arena_alloc_align(struct Arena* arena, size_t size, size_t align) 
{
	uintptr_t curr_ptr = (uintptr_t) arena->buff + (uintptr_t) arena->offset;
	uintptr_t offset = align_forward(curr_ptr, align);
	offset -= (uintptr_t) arena->buff; // Change to relative offset

	if (offset + size > arena->capacity) {
		fprintf(stderr, "Arena out of space! [capacity => %lu]\n", arena->capacity);
		return NULL;
	}

	void *ptr = arena->buff + offset;
	arena->offset = offset + size;

	return ptr;
}

struct Arena arena_create(byte* buffer, size_t size)
{
	return (struct Arena) {
		.buff = buffer,
		.offset = 0,
		.capacity = size
	};
}

void* arena_allocate(struct Arena* arena, size_t size)
{
	return arena_alloc_align(arena, size, DEFAULT_ALIGNMENT);
}

void arena_flush(struct Arena* arena)
{
	arena->offset = 0;
}

#endif // ARENA_ALLOC_IMPELEMENTATION
