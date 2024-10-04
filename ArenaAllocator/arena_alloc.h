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

#include <stdio.h>

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
	if (arena->offset + size > arena->capacity) {
		fprintf(stderr, "Arena out of space! [capacity => %lu]\n", arena->capacity);
		return NULL;
	}

	void* tmp = arena->buff + arena->offset;
	arena->offset += size;
	return tmp;
}

void arena_flush(struct Arena* arena)
{
	arena->offset = 0;
}

#endif // ARENA_ALLOC_IMPELEMENTATION
