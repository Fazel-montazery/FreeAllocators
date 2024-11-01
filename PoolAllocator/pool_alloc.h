#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef unsigned char byte;

// States
typedef enum {
	SUCCESS,
	ERROR_MEMORY_RESERVATION
} State;

// Pool structure
struct Pool {
	byte* buff;
	size_t capacity;
	size_t chunkSize;
	int64_t chunkCount;
	int64_t chunkAllocCount;
	int64_t alignment;
	uintptr_t* trackingStack;
};

// Api declaration
State pool_create(struct Pool* pool, int64_t chunkCount, size_t chunkSize, bool initZero);
State pool_create_align(struct Pool* pool, int64_t chunkCount, size_t chunkSize, int64_t alignment, bool initZero);
void* pool_allocate(struct Pool* pool);
void pool_free(struct Pool* pool, void* ptr);
void pool_flush(struct Pool* pool);
void pool_destroy(struct Pool* pool);

// Api implementation
#ifdef POOL_ALLOC_IMPLEMENTATION

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (sizeof(void *))
#endif

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

State pool_create_align(struct Pool* pool, int64_t chunkCount, size_t chunkSize, int64_t alignment, bool initZero)
{
	struct Pool p;
		
	chunkSize = align_forward((uintptr_t) chunkSize, alignment);
	size_t cap = (chunkCount * chunkSize) + alignment; // This additional alignment ensures accurate capacity calculation

	byte* buffer = mem_reserve(cap, initZero);
	if (!buffer)
		return ERROR_MEMORY_RESERVATION;

	uintptr_t chunkStart = align_forward((uintptr_t) buffer, alignment);

	uintptr_t* trackingStack = mem_reserve(chunkCount * sizeof(uintptr_t), false);
	for (uint64_t i = 0; i < chunkCount; i++) {
		trackingStack[i] = chunkStart + (i * chunkSize);
	}

	p.buff = buffer;
	p.capacity = cap;
	p.chunkSize = chunkSize;
	p.chunkCount = chunkCount;
	p.chunkAllocCount = 0;
	p.alignment = alignment;
	p.trackingStack = trackingStack;

	*pool = p;
	return SUCCESS;
}

State pool_create(struct Pool* pool, int64_t chunkCount, size_t chunkSize, bool initZero)
{
	return pool_create_align(pool, chunkCount, chunkSize, DEFAULT_ALIGNMENT, initZero);
}

void* pool_allocate(struct Pool* pool)
{
	return (pool->chunkCount == pool->chunkAllocCount) ? NULL : (void*) pool->trackingStack[pool->chunkAllocCount++];
}

void pool_free(struct Pool* pool, void* ptr)
{
	pool->trackingStack[--pool->chunkAllocCount] = (uintptr_t) ptr;
}

void pool_flush(struct Pool* pool)
{
	pool->chunkAllocCount = 0;
	uintptr_t chunkStart = align_forward((uintptr_t) pool->buff, pool->alignment);
	for (uint64_t i = 0; i < pool->chunkCount; i++) {
		pool->trackingStack[i] = chunkStart + (i * pool->chunkSize);
	}
}

void pool_destroy(struct Pool* pool)
{
	mem_free(pool->buff, pool->capacity);
	mem_free(pool->trackingStack, pool->chunkCount * sizeof(uintptr_t));
	pool->buff = NULL;
	pool->capacity = -1;
	pool->chunkSize = -1;
	pool->chunkCount = -1;
	pool->chunkAllocCount = -1;
	pool->trackingStack = NULL;
}

#endif // POOL_ALLOC_IMPLEMENTATION
