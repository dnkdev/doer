#include <stddef.h>

typedef struct
{
    void *ptr;
    size_t size;
} AllocatedMemory;

void *allocate_new(size_t size);