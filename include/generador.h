#ifndef GENERADOR_H
#define GENERADOR_H

#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <fcntl.h>
#include "utils.h"

extern sem_t *mSePideIDs, *mAccederSHM, *cantProductosBuffer, *capBuffer;
extern memoria_compartida_t* shm_base;


// Generador: solicita bloques de IDs y genera productos hasta completar su cuota
void *generador(void* arg);
// Simula el envío de un producto (fácil de migrar a SHM)
void enviar_producto(Producto *dest, Producto prod);

#endif // GENERADOR_H