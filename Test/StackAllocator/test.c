#define STACK_ALLOC_IMPELEMENTATION
#include "../../StackAllocator/stack_alloc.h"

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

	struct Stack stack = stack_create(buffer, BUFF_SIZE, false);

	printf("Everything OK.\n");

	free(buffer);
	return 0;
}
