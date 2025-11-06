#ifndef MESSAGE_H  
#define MESSAGE_H
#define SIMPLEMSG_SIZE 256 

#include <stdlib.h>
#include <sys/types.h> 

//struttura per il file
struct msgStruct {
    long mtype;                // Tipo del messaggio (richiesto da msgget/msgsnd/msgrcv)
    char text[SIMPLEMSG_SIZE];
    pid_t client_pid;
};

void printMessage1(struct msgStruct *msg);
void printMessage2(struct msgStruct *msg);
void printMessage3(struct msgStruct *msg);
void printMessage4(struct msgStruct *msg);



#endif