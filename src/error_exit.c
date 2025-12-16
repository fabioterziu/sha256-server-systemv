#include <stdio.h>
#include <errno.h> 
#include <stdlib.h> 

#include "error_exit.h"

void errExit(const char *msg) {
  fprintf(stderr, "%s\n", msg); //senza aggiunta messaggio errno
  exit(1);
}
