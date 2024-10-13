#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef unsigned char byte;

// Header structure
struct ChunkHeader {
	struct ChunkHeader* nextFree;
};

// Pool structure
struct Pool {
	byte* buff;
	int64_t chunkSize;
	int64_t capacity;

	struct ChunkHeader* head;
};

// Api declaration
bool pool_create_align(struct Pool* p, byte* buffer, int64_t bufferSize, int64_t chunkSize, uintptr_t chunkAlignment, bool initZero);
bool pool_create(struct Pool* p, byte* buffer, int64_t bufferSize, int64_t chunkSize, bool initZero);
void pool_flush(struct Pool* pool);

// Api implementation
#ifdef POOL_ALLOC_IMPELEMENTATION

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT 8
#endif

#ifndef POOL_ALLOC_NO_LOG
#include <stdio.h>
#endif

#include <string.h>

static uintptr_t align_forward(uintptr_t ptr, uintptr_t align) 
{
	uintptr_t p, a, modulo;

	p = ptr;
	a = align;
	modulo = p & (a - 1);

	if (modulo != 0) {
		p += a - modulo;
	}

	return p;
}

bool pool_create_align(struct Pool* p, byte* buffer, int64_t bufferSize, int64_t chunkSize, uintptr_t chunkAlignment, bool initZero)
{
	uintptr_t initStart = (uintptr_t) buffer;
	uintptr_t start = align_forward(initStart, (uintptr_t) chunkAlignment);
	bufferSize -= (int64_t) (start - initStart);

	chunkSize = align_forward((uintptr_t)chunkSize, chunkAlignment);
	if (chunkSize < sizeof(struct ChunkHeader)) {
#ifndef POOL_ALLOC_NO_LOG
		fprintf(stderr, "Chunk size is too small.\n");
#endif
		return false;
	}

	if (bufferSize < chunkSize) {
#ifndef POOL_ALLOC_NO_LOG
		fprintf(stderr, "Buffer size is too small.\n");
#endif
		return false;
	}

	if (initZero) memset(buffer, 0, bufferSize);

	p->buff = (byte*) buffer;
	p->capacity = bufferSize;
	p->chunkSize = chunkSize;
	p->head = NULL;

	pool_flush(p);
}

bool pool_create(struct Pool* p, byte* buffer, int64_t bufferSize, int64_t chunkSize, bool initZero) {
	return pool_create_align(p, buffer, bufferSize, chunkSize, DEFAULT_ALIGNMENT, initZero);
}

void pool_flush(struct Pool* pool)
{
	int64_t chunkCount = pool->capacity / pool->chunkSize;
	for (int64_t i = 0; i < chunkCount; i++) {
		void* ptr = &pool->buff[i * pool->chunkSize];
		struct ChunkHeader* header = (struct ChunkHeader*) ptr;
		header->nextFree = pool->head;
		pool->head = header;
	}
}

#endif // POOL_ALLOC_IMPELEMENTATION
