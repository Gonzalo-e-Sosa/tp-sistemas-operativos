#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "control_de_parametros.h"
#include "utils.h"
#include "semaforos.h"
#include "coordinador.h"
#include "generador.h"


#define SHM_NAME "/shm_productos_tp"
#define SHM_UNLINK 1

sem_t *mSePideIDs, *mAccederSHM, *cantProductosBuffer, *capBuffer;
memoria_compartida_t* shm_base = NULL;

int main(int argc, char *argv[]) {
    // utilizar parametros de entrada para generar N procesos generadores
    Configuracion config;
    char* archivo_salida = NULL;
    if(!parsear_parametros(argc, argv, &config) || config.help){
        mostrar_ayuda(argv[0]);
        return HELP;
    }
    archivo_salida = config.archivo_salida ? config.archivo_salida : DEFAULT_OUTPUT_FILE;
    printf("Modo verbose activado\n");
    printf("Configuración:\n");
    printf("  Generadores: %d\n", config.generadores);
    printf("  Registros: %d\n", config.registros);
    printf("  Archivo salida: %s\n", archivo_salida);
    // Inicializar semáforos
    inicializarSemaforos(config.verbose);
    int shm_fd;
    // Crear y mapear SHM (corregir tamaño)
    size_t shm_size = sizeof(memoria_compartida_t);
    shm_base = (memoria_compartida_t*)shm_create_and_map(SHM_NAME, shm_size, &shm_fd);

    // Inicializar memoria compartida
    inicializarMemoriaCompartida();

    // Crear proceso coordinador
    pid_t pCoordinador;
    pCoordinador = fork();
    if (pCoordinador < 0) {
        perror("fork");
        exit(1);
    }
    if (pCoordinador == 0) {
        // Proceso coordinador
        if(config.verbose)
            printf("Proceso coordinador iniciado (PID: %d)\n", getpid());
        coordinador((void*)&config);
        exit(0);
    }
    // Proceso padre (crea generadores)
    srand(time(NULL));
    pthread_t* threads = malloc(sizeof(pthread_t) * config.generadores);
    if(!threads){
        fprintf(stderr, "Error: no se pudo asignar memoria para los hilos\n");
        exit(1);
    }
    for (int i = 0; i < config.generadores; ++i)
    {
        pthread_create(&threads[i], NULL, generador, (void*)&config.verbose);
    }
    for (int i = 0; i < config.generadores; ++i)
    {
        pthread_join(threads[i], NULL);
    }
    // esperar a que termine el proceso coordinador
    waitpid(pCoordinador, NULL, 0);

    //liberar memoria reservada de hilos
    free(threads);

    shm_unmap_and_close(shm_base, shm_size, shm_fd, SHM_NAME, SHM_UNLINK);
    // Cerrar y eliminar semáforos
    destruirSemaforos(config.verbose);

    printf("Generación de datos finalizada. Archivo: %s\n", archivo_salida);
    if (config.archivo_salida) {
        free(config.archivo_salida);
    }
    return 0;
}
