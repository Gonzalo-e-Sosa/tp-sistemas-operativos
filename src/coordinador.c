#include "coordinador.h"

int hay_productos_en_buffer(){
    int res;
    sem_wait(mAccederSHM);
    res = shm_base->count > 0;
    sem_post(mAccederSHM);
    return res;
}
//proceso coordinador
void* coordinador(void* arg){
    int id_inicial = 0;
    int productos_generados = 0;
    char seCompleto = 0;
    Configuracion* config = (Configuracion*) arg;
    char* archivo_salida = config->archivo_salida ? config->archivo_salida : DEFAULT_OUTPUT_FILE;
    FILE* pFile = fopen(archivo_salida, "w");
    if (pFile == NULL) {
        perror("Error al abrir el archivo de salida");
        exit(1);
    }
    fprintf(pFile, "ID,Codigo,Descripcion,Lote,FechaIngreso,FechaVencimiento,Cantidad\n");
    fclose(pFile);

    while(productos_generados < config->registros){
        // 1. Atender pedidos de IDs
        if(!seCompleto){
            sem_wait(mAccederSHM);
            if(id_inicial >= config->registros){
                // ya no hay ids a asignar; solo falta que leer el bufer y terminar los hilos generadores
                shm_base->se_completo = 1;
                seCompleto = 1;
            } else if (shm_base->pedido_ids == 1 && shm_base->id_desde == -1) {
                shm_base->id_desde = id_inicial;
                shm_base->id_hasta = (id_inicial + BLOCK_IDS) < config->registros ? (id_inicial + BLOCK_IDS) : (config->registros);
                id_inicial = shm_base->id_hasta;
                if(config->verbose)
                    printf("Coordinador: asignando bloque de IDs desde %d hasta %d\n", shm_base->id_desde, shm_base->id_hasta);
            }
            shm_base->pedido_ids = 0; // Marcar pedido como atendido
            sem_post(mAccederSHM);
        }

        // 2. Atender buffer de productos
        if(hay_productos_en_buffer()){
            sem_wait(cantProductosBuffer);
            sem_wait(mAccederSHM);
            Producto prod = shm_base->buffer[shm_base->out];
            shm_base->out = (shm_base->out + 1) % TAM_BUFFER;
            shm_base->count--;
            sem_post(mAccederSHM);
            sem_post(capBuffer);
            coordinador_escribir_csv(archivo_salida, &prod);
            productos_generados++;
        }
        // usleep(1000);
    }
    if(config->verbose)
        printf("Coordinador: finalizó. Total productos escritos: %d\n", productos_generados);
    return NULL;
}
// Escritura de productos
void coordinador_escribir_csv(const char *filename, Producto* prod) {
    // Escribir producto en archivo
    FILE* pFile = fopen(filename, "a");
    if (pFile == NULL) {
        perror("Error al abrir el archivo de salida");
        exit(1);
    }
    print_producto(pFile, prod);
    fclose(pFile);
}