#include <stdio.h>

#include "sharedMemory.h"

void printShmem(struct shmemStruct *mem) {
    printf("path_len: %zu\n", mem->path_len);
}