#include <stdio.h>

#include "shared_memory.h"

void printShmem(struct shmemStruct *mem) {
    printf("path_len: %zu\n", mem->path_len);
}