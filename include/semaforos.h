#ifndef SEMAFOROS_H
#define SEMAFOROS_H

#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <fcntl.h>

extern sem_t *mSePideIDs, *mAccederSHM, *cantProductosBuffer, *capBuffer;

#define IDS_SEM "/sem_ids"
#define BUFFER_SEM "/sem_buffer"
#define CANT_REG_SEM "/sem_cant_reg"
#define CAPACIDAD_SEM "/sem_capacidad"

#define TAM_BUFFER 5 // Tamaño del buffer circular

void inicializarSemaforos(const int verbose);
void destruirSemaforos(const int verbose);

#endif // SEMAFOROS_H