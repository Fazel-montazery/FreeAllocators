#define ARENA_ALLOC_IMPELEMENTATION
#include "../../ArenaAllocator/arena_alloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define BUFF_SIZE 1000

int main(int argc, char** argv)
{
	byte* buffer = malloc(BUFF_SIZE);
	assert(buffer != NULL);

	
	struct Arena arena = arena_create(buffer, BUFF_SIZE);
	assert(arena.buff == buffer);
	assert(arena.capacity == BUFF_SIZE);
	assert(arena.offset == 0);
	printf("Arena Creation OK.");



	free(buffer);
	return 0;
}
