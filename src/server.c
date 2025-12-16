#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/shm.h> 
#include <unistd.h>

#include "message.h"
#include "error_exit.h" 
#include "shared_memory.h"
#include "sha_256.h"

//DICHIARAZIONI GLOBALI
int qid = -1;                    //id coda
int shmflg = IPC_CREAT | 0666;   //se non esiste segmento con la chiave specificata, crealo
size_t Serverpath_len = 0;       //dimensione del path
struct msgStruct strutturamsg;   //struttura per il messaggio
//struct shmemStruct structmem;         //popola struttura per la memoria condivisa



//DICHIARAZIONE FUNZIONI
void checkArguments(int argc, char *argv[]);                 //controllo argomenti
key_t generateKey(const char *pathname, int id);             //genera chiave
int queue_id(key_t key);                                     //crea coda se non esiste e restituisce idenfificatore
int shared_mem(key_t key, size_t size, int shmflg);          //crea memoria condivisa
void signTermHandler(int sig);                               //segnali


int main(int argc, char *argv[]) {

    // GESTIONE SEGNALI
    if (signal(SIGTERM, signTermHandler) == SIG_ERR) //termina in kodo ordinato
        errExit("change SIGTERM handler failed");
    if (signal(SIGINT, signTermHandler) == SIG_ERR) //interrupt
        errExit("change SIGINT handler failed");
    if (signal(SIGHUP, signTermHandler) == SIG_ERR) //ricarica il processo
        errExit("change SIGHUP handler failed");


    checkArguments( argc, argv);                                             //controllo argomenti
    key_t key = generateKey("generate.keyfile", 71);                         //genera chiave

    qid = queue_id(key);                                                   //crea coda se non esiste e restituisce idenfificatore coda
    printf("<SERVER> MSG queue created: waiting for messages...\n");

    int shmid = shared_mem(key, sizeof(struct shmemStruct), shmflg);         //crea memoria condivisa. ritorna id mem
    void *sh_mem = shmat(shmid, NULL, 0);                                    //collega memoria condivisa
    if (sh_mem == (void *) -1)
        errExit("<SERVER> shmat failed"); //shared mem attach fallita


    while (1) {
        size_t msgSize = sizeof(struct msgStruct) - sizeof(long);        //dimensione messaggio

        //MESSAGGI CONNESSIONE
        if (msgrcv(qid, &strutturamsg, msgSize, 1, 0) == -1)
            errExit("<SERVER> msgrcv failed");
        printf("<SERVER> Connection request recived from CLIENT%d\n", strutturamsg.client_pid);

        strutturamsg.mtype = 2;                                         //tipo 2                
        strncpy(strutturamsg.text, "ACK", SIMPLEMSG_SIZE - 1);          //messaggio DA INVIARE

        if (msgsnd(qid, &strutturamsg, msgSize, 2) == -1)               //invio 
            errExit("<SERVER> msgsnd failed");
    
         printf("<SERVER> ACK sent.\n");                                //conferma invio



        //STRUTTURA MEM CONDIVISA
        //size_t shmemSize = sizeof(struct shmemStruct) - sizeof(long);  // dimensione messaggio
        //structmem.path_len = 0; //popolo struttura per la memoria condivisa
        //structmem.path[0] = '\0'; //inizializzo path a stringa vuota

        //STAMPA
        struct shmemStruct *shared = (struct shmemStruct *)sh_mem;
        printf("<SERVER> Path length: %zu\n", shared->path_len);
        printf("<SERVER> Path: %s\n", shared->path);


    uint8_t hash[32];  // 256 bit = 32 byte
    digest_file(shared->path, hash);
    for (int i = 0; i < 32; i++){
        printf("%02x", hash[i]);
    }
    printf("\n");

    //

    }
    return 0;
}


//CONTROLLO ARGOMENTI
void checkArguments(int argc, char *argv[]){
    if (argc != 1) { //solo 1
        printf("<SERVER> Usage only: %s\n", argv[0]);
        exit(1);
    }
}


// GENERA CHIAVE IPC
key_t generateKey(const char *pathname, int id) {
    key_t key = ftok(pathname, id);
    if (key == -1) {
        perror("<SERVER> ftok failed");
        exit(1);
    }
    return key;
}


// CREA CODA SE NON ESISTE E RESTITUISCE IDENFIFICATORE CODA
int queue_id(key_t key) {
    int msqid = msgget(key, IPC_CREAT | S_IRUSR | S_IWUSR);
    if (msqid == -1)
        errExit("<SERVER> msgget failed");
    return msqid;
}


//CREA MEMORIA CONDIVISA
int shared_mem(key_t key, size_t size, int shmflg){ 
    int shmid = shmget(key, size, shmflg);  // identificatore della memoria condivisa
    if (shmid == -1) { //se errore creazione memoria
        perror("<SERVER> Shared memory error");
        exit(1);
    }
    return shmid; //ritorno id mem condivisa
}


//SEGNALI 
void signTermHandler(int sig) {
    if (qid > 0) {
        if (msgctl(qid, IPC_RMID, NULL) == -1) //elimina coda di messaggi
            errExit("msgctl failed");
        else
            printf("<Server> message queue removed successfully\n");
    }
    exit(0); //termina processo server
}
