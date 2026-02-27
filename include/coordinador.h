#ifndef COORDINADOR_H
#define COORDINADOR_H

#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <fcntl.h>
#include "control_de_parametros.h"
#include "utils.h"

#define BLOCK_IDS 10

extern sem_t *mSePideIDs, *mAccederSHM, *cantProductosBuffer, *capBuffer;
extern memoria_compartida_t* shm_base;

int hay_productos_en_buffer();
void* coordinador(void* arg);
void coordinador_escribir_csv(const char *filename, Producto* prod);


#endif // COORDINADOR_H