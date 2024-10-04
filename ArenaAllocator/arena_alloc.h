#pragma once

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

#endif // ARENA_ALLOC_IMPELEMENTATION
