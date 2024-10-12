#pragma once

typedef unsigned char byte;

// Header structure
struct ChunkHeader {
	struct Header* nextFree;
};

// Pool structure
struct Pool {
	byte* buff;
	int64_t chunkSize;
	int64_t capacity;

	struct ChunkHeader* head;
};

// Api declaration
struct Pool pool_create(byte* buffer, int64_t bufferSize, int64_t chunkSize, bool initZero);

// Api implementation
#ifdef POOL_ALLOC_IMPELEMENTATION

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

#ifndef POOL_ALLOC_NO_LOG
#include <stdio.h>
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

#endif // POOL_ALLOC_IMPELEMENTATION
