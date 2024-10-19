#define STACK_ALLOC_IMPELEMENTATION
#define STACK_ALLOC_NO_LOG
#include "../../StackAllocator/stack_alloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define BUFF_SIZE 1000
#define NEW_BUFF_SIZE 2000

int main(int argc, char** argv)
{
	struct Stack stack;
	assert(stack_create(&stack, BUFF_SIZE, false) == SUCCESS);
	assert(stack.offset == 0);
	assert(stack.capacity == BUFF_SIZE + stack.extra);
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

	int32_t* arr4 = stack_allocate(&stack, 100 * sizeof(int32_t));
	assert(arr4 != NULL);
	assert(stack.size == 100 * sizeof(int32_t));
	printf("Stack Allocation[5] OK.\n");

	stack_flush(&stack);
	assert(stack.currHeader == NULL);
	assert(stack.offset == 0);
	printf("Stack Flush OK.\n");

	int32_t* arr5 = stack_allocate(&stack, 100 * sizeof(int32_t));
	assert(arr5 != NULL);
	assert(stack.size == 100 * sizeof(int32_t));
	printf("Stack Allocation[6] OK.\n");

	stack_shrink(&stack, 900);
	arr5 = stack_allocate(&stack, 101);
	assert(arr5 == NULL);
	assert(stack.size == 0);
	arr5 = stack_allocate(&stack, 80);
	assert(arr5 != NULL);
	assert(stack.size == 80);
	printf("Stack Shrink OK.\n");
	
	byte* newBuffer = malloc(NEW_BUFF_SIZE);
	assert(newBuffer != NULL);
	stack_updtae_buffer(&stack, newBuffer, NEW_BUFF_SIZE, false);
	assert(stack.buff == newBuffer);
	assert(stack.capacity == NEW_BUFF_SIZE);
	assert(stack.offset == 0);
	printf("Stack Change Buffer OK.\n");

	stack_destroy(&stack);
	assert(stack.offset == -1);
	printf("Stack Destruction OK.\n");

	printf("Everything OK.\n");

	return 0;
}
