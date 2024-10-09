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
	assert(stack.buff == buffer);
	assert(stack.offset == 0);
	assert(stack.capacity == BUFF_SIZE);
	printf("Stack Creation OK.\n");

	int32_t* arr1 = stack_allocate(&stack, 100 * sizeof(int32_t));
	assert(arr1 != NULL);
	assert(stack.size == 100 * sizeof(int32_t));
	printf("Stack Allocation[1] OK.\n");

	int32_t* arr2 = stack_allocate(&stack, 400 * sizeof(int32_t));
	assert(arr2 == NULL);
	assert(stack.size == 100 * sizeof(int32_t));
	printf("Stack Allocation[2] OK.\n");

	arr2 = stack_allocate(&stack, 50 * sizeof(int32_t));
	assert(arr2 != NULL);
	assert(stack.size == 150 * sizeof(int32_t));
	printf("Stack Allocation[3] OK.\n");

	int32_t* arr3 = stack_allocate(&stack, 17 * sizeof(int32_t));
	assert(arr3 != NULL);
	assert(stack.size == 167 * sizeof(int32_t));
	printf("Stack Allocation[4] OK.\n");

	stack_pop(&stack);
	assert(stack.size == 150 * sizeof(int32_t));
	stack_pop(&stack);
	assert(stack.size == 100 * sizeof(int32_t));
	stack_pop(&stack);
	assert(stack.size == 0);
	assert(stack.offset == 0);
	assert(stack.currHeader == NULL);
	printf("Stack Pop OK.\n");

	printf("Everything OK.\n");

	free(buffer);
	return 0;
}
