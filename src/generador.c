#include "generador.h"

// Simula el envío de un producto (fácil de migrar a SHM)
void enviar_producto(Producto *dest, Producto prod)
{
    memcpy(dest, &prod, sizeof(Producto));
}
// Generador: solicita bloques de IDs y genera productos hasta completar su cuota
void *generador(void* arg) {
    char verbose = *((char*)arg);
    int generados = 0;
    int id_inicial = -1;
    int id_final;
    Producto prod;
    pthread_t tid = pthread_self();
    char seTermino = 0;
    while (!seTermino) {
        // 1. Solicitar bloque de IDs al coordinador
        sem_wait(mSePideIDs);
        sem_wait(mAccederSHM);
        if(1 == shm_base->se_completo){
            seTermino = 1;
            if(verbose)
                printf("Generador %lu: Intento pedir IDs, pero ya no hay IDs disponibles\n", tid);
        } else if (shm_base->pedido_ids == 0) {
            shm_base->pedido_ids = 1;
            shm_base->id_desde = -1; // Marcar como pendiente
        }
        sem_post(mAccederSHM);
        // 2. Esperar a que el coordinador asigne el bloque de IDs
        do {
            sem_wait(mAccederSHM);
            id_inicial = shm_base->id_desde;
            id_final = shm_base->id_hasta;
            shm_base->id_desde = -1; // Marcar como leído
            sem_post(mAccederSHM);
            // usleep(100);
        } while (!seTermino && id_inicial == -1);
        sem_post(mSePideIDs);
        // 3. Generar productos con los IDs asignados
        if(verbose && !seTermino)
            printf("Generador (Thread %lu): generando productos con IDs desde %d a %d\n", tid, id_inicial, id_final);
        for (int i = id_inicial; !seTermino && i < id_final; ++i) {
            prod = generar_producto(i);
            prod.id = i;
            // Enviar producto al buffer compartido
            sem_wait(capBuffer);
            sem_wait(mAccederSHM);
            enviar_producto(&shm_base->buffer[shm_base->in], prod);
            shm_base->in = (shm_base->in + 1) % TAM_BUFFER;
            shm_base->count++;
            sem_post(mAccederSHM);
            sem_post(cantProductosBuffer);
            generados++;
        }
    }
    if(verbose)
        printf("Generador (Thread %lu): finalizó. Total generados: %d\n", tid, generados);
    pthread_exit(NULL);
}
