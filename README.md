# FreeAllocators
A collection of cross-platform memory allocators and managers with runtime memory alignment support, written in a single-header file style.

## Usage
Simply define `[AllocatorName]_ALLOC_IMPLEMENTATION` in a single C file, and compile it with your project source code.
for example:
```C
#define ARENA_ALLOC_IMPLEMENTATION
#include "arena_alloc.h"
```
You could also define `[AllocatorName]_ALLOC_NO_LOG` in order to get rid of the debug messages.

**Notice:** Like `malloc` and `free` in the libc, this implementation does not guarantee correct behavior if a pointer is freed multiple times or if a NULL pointer is freed.

## Examples
See test files for more info.

## License
This repo is licensed under the MIT License.
