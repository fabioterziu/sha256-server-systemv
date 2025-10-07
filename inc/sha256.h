#ifndef SHA256_UTILS_H
#define SHA256_UTILS_H
#include <stdint.h>     // per uint8_t

// Funzione che calcola l'impronta SHA-256 di un file
void digest_file(const char *filename, uint8_t *hash);

#endif