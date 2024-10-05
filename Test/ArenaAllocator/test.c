#define ARENA_ALLOC_IMPELEMENTATION
#include "../../ArenaAllocator/arena_alloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define BUFF_SIZE 1000
#define NEW_BUFF_SIZE 2000

int main(int argc, char** argv)
{
	byte* buffer = malloc(BUFF_SIZE);
	assert(buffer != NULL);

	struct Arena arena = arena_create(buffer, BUFF_SIZE, false);
	assert(arena.buff == buffer);
	assert(arena.capacity == BUFF_SIZE);
	assert(arena.size == 0);
	printf("Arena Creation OK.\n");

	int32_t* arr1 = arena_allocate(&arena, sizeof(int32_t) * 100);
	assert(arr1 != NULL);
	assert(arena.size == (sizeof(int32_t) * 100));
	printf("Arena Allocation[1] OK.\n");

	int32_t* arr2 = arena_allocate(&arena, sizeof(int32_t) * 150);
	assert(arr2 != NULL);
	assert(arena.size == (sizeof(int32_t) * 250));
	printf("Arena Allocation[2] OK.\n");

	int32_t* arr3 = arena_allocate(&arena, sizeof(int32_t));
	assert(arr3 == NULL);
	assert(arena.size == (sizeof(int32_t) * 250));
	printf("Arena Allocation[3] OK.\n");

	arena_flush(&arena);
	assert(arena.size == 0);
	printf("Arena Flush OK.\n");

	int32_t* arr4 = arena_allocate(&arena, sizeof(int32_t) * 250);
	assert(arr4 != NULL);
	assert(arena.size == (sizeof(int32_t) * 250));
	printf("Arena Allocation[4] OK.\n");

	arena_shrink(&arena, 250);
	assert(arena.capacity == 750);
	printf("Arena shrink[1] OK.\n");

	arena_shrink(&arena, 850);
	assert(arena.capacity == 750);
	printf("Arena shrink[2] OK.\n");

	byte* tmp = realloc(buffer, NEW_BUFF_SIZE);
	assert(tmp != NULL);
	buffer = tmp;
	arena_update_buffer(&arena, buffer, NEW_BUFF_SIZE, false);
	assert(arena.capacity == NEW_BUFF_SIZE);
	assert(arena.offset == 0);
	printf("Arena Change Buffer OK.\n");

	printf("Everything OK.\n");

	free(buffer);
	return 0;
}
