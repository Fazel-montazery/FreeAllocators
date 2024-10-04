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

struct Arena arena_create(byte* buffer, size_t size)
{
	return (struct Arena) {
		.buff = buffer,
		.offset = 0,
		.capacity = size
	};
}


#endif // ARENA_ALLOC_IMPELEMENTATION
