#ifndef SHARED_MEMORY_H //se non è definito
#define SHARED_MEMORY_H //definisci ora

#include <stdlib.h>
#include <sys/types.h> 


struct shmemStruct {
    size_t path_len; //dimensione path
    
    char path[256]; //

};

void printShmem(struct shmemStruct *mem);


#endif