#define POOL_ALLOC_IMPELEMENTATION
#include "../../PoolAllocator/pool_alloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

typedef struct {
	int32_t a;
	int32_t b;
	int32_t c;
} MyStruct;

#define BUFF_SIZE (10 * sizeof(MyStruct))
#define NEW_BUFF_SIZE (20 * sizeof(MyStruct))

int main(int argc, char** argv)
{
	byte* buffer = malloc(BUFF_SIZE);
	
	struct Pool pool;
	assert(pool_create(&pool, buffer, BUFF_SIZE, sizeof(MyStruct), false));
	assert(pool.buff == buffer);
	printf("Pool Creation OK.\n");

	MyStruct* s1 = pool_alloc(&pool);
	assert(s1 != NULL);
	printf("Pool Allocation[1] OK.\n");

	MyStruct* s2 = pool_alloc(&pool);
	assert(s2 != NULL);
	assert(s1 != s2);
	printf("Pool Allocation[2] OK.\n");

	MyStruct* s_arr[6];
	for (int i = 0; i < 6; i++) {
		s_arr[i] = pool_alloc(&pool);
		assert(s_arr[i] != NULL);
	}
	printf("Pool Allocation[3] OK.\n");


	printf("Everything OK.\n");

	free(buffer);
	return 0;
}
