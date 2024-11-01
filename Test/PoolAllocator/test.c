#define POOL_ALLOC_IMPLEMENTATION
#include "../../PoolAllocator/pool_alloc.h"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

struct testChunk {
	int64_t a;
	int32_t b;
	unsigned char c;
};

typedef struct testChunk test_t;

#define CHUNK_COUNT 5
#define CHUNK_SIZE (sizeof(struct testChunk))

int main(int argc, char** argv)
{
	struct Pool pool = {0};
	assert(pool_create(&pool, CHUNK_COUNT, CHUNK_SIZE, false) == SUCCESS);
	assert(pool.buff != NULL);
	assert(pool.trackingStack != NULL);
	assert(pool.chunkCount == CHUNK_COUNT);
	printf("Pool Creation OK.\n");

	test_t* t1 = pool_allocate(&pool);
	assert(t1 != NULL);
	assert(pool.chunkAllocCount == 1);
	printf("Pool Allocation[1] OK.\n");

	test_t* t2[4];
	for (int i = 0; i < 4; i++) {
		t2[i] = pool_allocate(&pool);
		assert(t2[i] != NULL);
	}
	assert(pool.chunkAllocCount == 5);
	printf("Pool Allocation[2] OK.\n");

	test_t* t3 = pool_allocate(&pool);
	assert(t3 == NULL);
	assert(pool.chunkAllocCount == 5);
	printf("Pool Allocation[3] OK.\n");

	test_t* tmp = t1;
	pool_free(&pool, t1);
	assert(pool.chunkAllocCount == 4);
	printf("Pool DeAllocation[1] OK.\n");

	test_t* t4 = pool_allocate(&pool);
	assert(t4 != NULL);
	assert(pool.chunkAllocCount == 5);
	assert(tmp == t4);
	printf("Pool Allocation[4] OK.\n");

	pool_flush(&pool);
	assert(pool.chunkAllocCount == 0);
	printf("Pool Flush OK.\n");

	test_t* t5[5];
	for (int i = 0; i < 5; i++) {
		t5[i] = pool_allocate(&pool);
		assert(t5[i] != NULL);
	}
	assert(pool.chunkAllocCount == 5);
	printf("Pool Allocation[5] OK.\n");

	pool_destroy(&pool);
	assert(pool.buff == NULL);
	assert(pool.trackingStack == NULL);
	printf("Pool Destruction OK.\n");

	printf("Everything OK.\n");
	return 0;
}
