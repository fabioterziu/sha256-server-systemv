#ifndef SHAREDMEMORY_H      // per evitare inclusioni multiple (include guard)
#define SHAREDMEMORY_H

#include <stdlib.h>
#include <sys/types.h> 


struct shmemStruct {
    size_t path_len; //dimensione path
    
    char path[256]; //

};

void printShmem(struct shmemStruct *mem);


#endif