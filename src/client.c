#include <stdio.h>
#include <stdlib.h>     
#include <errno.h>
#include <string.h>
#include <sys/stat.h>   
#include <sys/msg.h>    
#include <sys/shm.h>    
#include <unistd.h>     
#include <limits.h>     
#include "message.h"
#include "sharedMemory.h"
#include "errExit.h"
#include "sha256.h" 


//DICHIARAZIONI GLOBALI
int shmflg = 0666;                     //solo permessi
struct msgStruct strutturamsg;         //struttura messaggio

//DICHIARAZIONE FUNZIONI
char *getPath(const char *fileName);                        //calcolo path
key_t generateKey(const char *fileName, int id);            //genera chiave IPC
int queue_id(key_t key);                                    //idenfificatore coda
int shared_mem(key_t key, size_t size, int shmflg);         //cerca memoria condivisa
void write_path(void *sh_mem, char *path);

int main (int argc, char *argv[]) { 

    //CONTROLLO ARGOMENTI
    if (argc != 2) {                                          //+ file                              
        printf("<CLIENT> Usage: %s file.txt\n", argv[0]);
        exit(1);
    }
    char *fName = argv[1];                                    //nome file

    //PID CLIENT
    pid_t my_pid = getpid();                           //ottengo pid client

    //PATH
    char *abs = getPath(fName);                         //calcolo path assoluto del file
    printf("%s\n", abs);                                //STAMPA DI DEBUG
    

    key_t key = generateKey("generate.keyfile", 71);       //genera chiave
    int qid = queue_id(key);                               //idenfificatore coda

    //STRUTTURA MESSAGGIO
    strutturamsg.mtype = 1;                                          //definisco struttura msg (client 1, server 2) 
    strutturamsg.client_pid = my_pid;                 
    snprintf(strutturamsg.text, SIMPLEMSG_SIZE - 1, "CLIENT %d", my_pid);////////messaggio DA INVIARE (indico chi sono)DA MODIFICARE, DIRÀ SEMPRE 1
    size_t msgSize = sizeof(struct msgStruct) - sizeof(long);        //dimensione messaggio

    //MESSAGGI PRIMA CONNESSIONE
    if (msgsnd(qid, &strutturamsg, msgSize, 1) == -1)                       //invio richiesta di connessione al server
        errExit("<CLIENT> msgsnd failed");
    printf("<CLIENT> Connection request sent.\n");                          //conferma invio (client video)
    
    if (msgrcv(qid, &strutturamsg, msgSize, 2, 0) == -1)                    //aspetto risposta dal server
            errExit("<CLIENT> msgrcv failed");      
    printf("<CLIENT> Connection confirmed: %s\n", strutturamsg.text);       //confermo a video messaggio server ricevuto

    //MEMORIA CONDIVISA
    int shmid = shared_mem(key, sizeof(struct shmemStruct), shmflg);    //cerca memoria condivisa, ritorna id memoria
    void *sh_mem = shmat(shmid, NULL, 0);                               //collega memoria condivisa
    if (sh_mem == (void *) -1)
        errExit("<CLIENT> shmat failed");

    //INVIO TRAMITE MEMORIA
    write_path(sh_mem, abs);  
    printf("<CLIENT> PATH inviato.\n");
    shmdt(sh_mem);  // stacca la memoria condivisa dopo la scrittura


    free(abs);                                          //libero memoria
    exit(0);

    //ricorda di chiudere coda memoria o quello che serve chiuder einsomma. in ogni funzione o punto di uscita!!!!!!!!!!!!!!!!!!!!!!!
}


//RESTITUISCE PATH ASSOLUTO FILE 
char *getPath(const char *fileName) { //filename = nome.txt
    //controllo allocazione
    char *absPath = malloc(PATH_MAX);           
    if (absPath == NULL) {
        perror("<CLIENT> malloc failed");
        exit(1);                                
    }
    //calcolo path assoluto
    if (realpath(fileName, absPath) == NULL) { 
        perror("<CLIENT> abspath failed");
        free(absPath); //libera se fallisce
        exit(1);
    }
    return absPath;
}

//RESTITUISCE CHIAVE IPC
key_t generateKey(const char *fileName, int id){ //filename = generate.keyfile, id = 71
    key_t key = ftok(fileName, id);
    if(key== -1){
        perror("ftok failed");
        exit(1);
    }
    return key; //restituisco chiave generata
}

//RESTITUISCE ID CODA (creato dal server)
int queue_id(key_t key){ 
    int msqid = msgget(key, S_IRUSR | S_IWUSR);
    if (msqid == -1)
        errExit("<CLIENT> Server not active");

    return msqid; //ritorno id coda
}

//RESTITUISCE ID MEMORIA CONDIVISA (creato dal server)
int shared_mem(key_t key, size_t size, int shmflg){ 
    int shmid = shmget(key, size, shmflg);  // identificatore della memoria condivisa
    if (shmid == -1) { //se errore creazione memoria
        perror("<SERVER> Shared memory error");
        exit(1);
    }
    return shmid; //ritorno id mem condivisa
}

//COLLEGA MEMORIA CONDIVISA E COPIA PATH
void write_path(void *sh_mem, char *path) {
    struct shmemStruct *shared = (struct shmemStruct *)sh_mem; // cast alla struttura shmemStruct

    size_t len = strlen(path); // Calcola la lunghezza del path

    if (len >= sizeof((*shared).path)) { //
        fprintf(stderr, "<CLIENT> Path too long\n");
        exit(1);
    }

    strncpy((*shared).path, path, sizeof((*shared).path) - 1);
    (*shared).path[sizeof((*shared).path) - 1] = '\0';
    (*shared).path_len = len;
}


