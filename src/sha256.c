#include <stdio.h>
#include <fcntl.h>      // per O_RDONLY
#include <unistd.h>     // per read(), close()
#include <stdlib.h>     // per exit()
#include <stdint.h>     // per uint8_t
#include <openssl/sha.h> // per SHA256

#include "sha256.h"

// x compilare
// clang -o mio_programma main.c sha256.c -I/opt/homebrew/opt/openssl@3/include -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto

void digest_file(const char *filename, uint8_t *hash){   //filename: nome del file da cui leggere i dati
	                                                      //hash: un array di 32 byte dove scriverà il digest SHA-256 risultante
   SHA256_CTX ctx; //creazione contesto
   SHA256_Init(&ctx); //inizializzazione contesto

   char buffer[32]; //buffer da 32 byte per leggere il file

   int file = open(filename, O_RDONLY, 0); //apri il file solo in lettura
   if (file == -1){ //controlla se il file è stato aperto correttamente
      printf("File %s does not exist\n", filename);
      exit(1); //se il file non esiste, stampa un messaggio di errore e termina il programma
   }

   ssize_t bR;
   do{
      // read the file in chunks of 32 characters
      bR = read(file, buffer, 32); //legge il file in blocchi di 32 byte
      if (bR > 0){ //se la lettura è andata a buon fine
         SHA256_Update(&ctx,(uint8_t *)buffer, bR); //aggiorna il contesto SHA-256 con i dati letti
      }
      else if (bR < 0){ //se la lettura ha fallito
         printf("Read failed\n");
         exit(1); //stampa un messaggio di errore e termina il programma
      }
   } while (bR > 0); //continua a leggere finché ci sono dati da leggere

   SHA256_Final(hash, &ctx); //completa il calcolo dell’hash e lo scrive nel buffer hash.

   close(file); //chiude il file
}