#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef unsigned char byte;

// States
typedef enum {
	SUCCESS,
	ERROR_MEMORY_RESERVATION
} State;

// Api declaration

// Api implementation
#ifdef POOL_ALLOC_IMPLEMENTATION

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
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

#endif // POOL_ALLOC_IMPLEMENTATION
