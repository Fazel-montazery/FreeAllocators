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
void* pool_alloc(struct Pool *p);
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

void* pool_alloc(struct Pool *p)
{
	struct ChunkHeader* ptr = p->head;

	if (!ptr) {
#ifndef POOL_ALLOC_NO_LOG
		fprintf(stderr, "No free memory available on the pool.\n");
#endif
		return NULL;
	}

	p->head = p->head->nextFree;
	return ptr;
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
