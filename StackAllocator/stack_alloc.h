#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef unsigned char byte;

// Stack structure
struct Stack {
	byte* buff;
	int64_t size; // offset without padding
	int64_t offset;
	int64_t capacity;
};

typedef int32_t _header_t;

// Api declaration
struct Stack stack_create(byte* buffer, int64_t size, bool initZero);

// Api implementation
#ifdef STACK_ALLOC_IMPELEMENTATION

#include <string.h>

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

struct Stack stack_create(byte* buffer, int64_t size, bool initZero)
{
	if (initZero) memset(buffer, 0, size);
	return (struct Stack) {
		.buff = buffer,
		.size = 0,
		.offset = 0,
		.capacity = size
	};
}

#endif // STACK_ALLOC_IMPELEMENTATION
