#include "semaforos.h"

void inicializarSemaforos(const int verbose) {
    // Elimina semáforos previos si existen (limpieza)
    sem_unlink(IDS_SEM);
    sem_unlink(BUFFER_SEM);
    sem_unlink(CANT_REG_SEM);
    sem_unlink(CAPACIDAD_SEM);

    // Crear e inicializar semáforos
    mSePideIDs = sem_open(IDS_SEM, O_CREAT, 0644, 1);
    if (mSePideIDs == SEM_FAILED) {
        perror("Error creando semáforo IDS_SEM");
        exit(EXIT_FAILURE);
    }

    mAccederSHM = sem_open(BUFFER_SEM, O_CREAT, 0666, 1);
    if (mAccederSHM == SEM_FAILED) {
        perror("Error creando semáforo BUFFER_SEM");
        exit(EXIT_FAILURE);
    }

    cantProductosBuffer = sem_open(CANT_REG_SEM, O_CREAT, 0666, 0);
    if (cantProductosBuffer == SEM_FAILED) {
        perror("Error creando semáforo CANT_REG_SEM");
        exit(EXIT_FAILURE);
    }

    capBuffer = sem_open(CAPACIDAD_SEM, O_CREAT, 0666, TAM_BUFFER);
    if (capBuffer == SEM_FAILED) {
        perror("Error creando semáforo CAPACIDAD_SEM");
        exit(EXIT_FAILURE);
    }
    if (verbose)
        printf("✅ Semáforos creados e inicializados correctamente.\n");
}
void destruirSemaforos(const int verbose) {
    sem_close(mSePideIDs);
    sem_close(mAccederSHM);
    sem_close(cantProductosBuffer);
    sem_close(capBuffer);

    sem_unlink(IDS_SEM);
    sem_unlink(BUFFER_SEM);
    sem_unlink(CANT_REG_SEM);
    sem_unlink(CAPACIDAD_SEM);
    if (verbose)
        printf("🧹 Semáforos eliminados correctamente.\n");
}
