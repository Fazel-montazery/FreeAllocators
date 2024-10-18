#define ARENA_ALLOC_IMPELEMENTATION
#define ARENA_ALLOC_NO_LOG
#include "../../ArenaAllocator/arena_alloc.h"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define BUFF_SIZE 1000
#define NEW_BUFF_SIZE 2000

int main(int argc, char** argv)
{
	struct Arena arena;
	assert(arena_create(&arena, BUFF_SIZE, false) == SUCCESS);
	assert(arena.capacity == BUFF_SIZE + EXTRA_CAP);
	assert(arena.size == 0);
	memset(arena.buff, '*', arena.capacity);
	for (int i = 0; i < arena.capacity; i++) assert(arena.buff[i] == '*');
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
	assert(arena.capacity == 750 + EXTRA_CAP);
	printf("Arena shrink[1] OK.\n");

	arena_shrink(&arena, 850);
	assert(arena.capacity == 750 + EXTRA_CAP);
	printf("Arena shrink[2] OK.\n");

	assert(arena_update_buffer(&arena, NEW_BUFF_SIZE, false) == SUCCESS);
	assert(arena.offset == 0);
	assert(arena.capacity == NEW_BUFF_SIZE + EXTRA_CAP);
	printf("Arena Buffer Change OK.\n");

	int32_t* arr5 = arena_allocate(&arena, sizeof(int32_t) * 400);
	assert(arr5 != NULL);
	assert(arena.size == (sizeof(int32_t) * 400));
	printf("Arena Allocation[5] OK.\n");

	arena_destroy(&arena);
	assert(arena.size == -1);
	printf("Arena Destruction OK.\n");

	printf("Everything OK.\n");
	return 0;
}
