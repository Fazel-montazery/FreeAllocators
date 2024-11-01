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

#define CHUNK_COUNT 100
#define CHUNK_SIZE (sizeof(struct testChunk))

int main(int argc, char** argv)
{
	struct Pool pool = {0};
	assert(pool_create(&pool, CHUNK_COUNT, CHUNK_SIZE, false) == SUCCESS);
	assert(pool.buff != NULL);
	assert(pool.trackingStack != NULL);
	assert(pool.chunkCount == CHUNK_COUNT);
	printf("Pool Creation OK.\n");


	pool_destroy(&pool);
	assert(pool.buff == NULL);
	assert(pool.trackingStack == NULL);
	printf("Pool Destruction OK.\n");

	printf("Everything OK.\n");
	return 0;
}
