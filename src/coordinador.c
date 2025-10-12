#include "coordinador.h"

int hay_productos_en_buffer(){
    int res;
    sem_wait(mAccederSHM);
    res = shm_base->count > 0;
    sem_post(mAccederSHM);
    return res;
}
int encontrar_hueco_id(char* ids_generados, int size, int* id_init, int* id_end){
    int i = 0;
    while(i < size && ids_generados[i] != 0)
        i++;
    if(i == size)
        return 0;
    *id_init = i;
    while(i < size && ids_generados[i] == 0)
        i++;
    *id_end = i;
    return 1;
}
//proceso coordinador
void* coordinador(void* arg){
    int id_inicial = 0;
    int productos_generados = 0;
    char seCompleto = 0;
    char* ids_generados = NULL;
    Configuracion* config = (Configuracion*) arg;
    char* archivo_salida = config->archivo_salida ? config->archivo_salida : DEFAULT_OUTPUT_FILE;
    FILE* pFile = fopen(archivo_salida, "w");
    if (pFile == NULL) {
        perror("Error al abrir el archivo de salida");
        exit(1);
    }
    fprintf(pFile, "ID,Codigo,Descripcion,Lote,FechaIngreso,FechaVencimiento,Cantidad\n");
    fclose(pFile);
    //control de progreso
    ids_generados = (char*) calloc(config->registros, sizeof(char));
    if(ids_generados == NULL){
        perror("Error al asignar memoria para el control de IDs generados");
        exit(1);
    }
    memset(ids_generados, 0, config->registros * sizeof(char));
    // Bucle principal del coordinador
    while(productos_generados < config->registros){
        // 1. Atender pedidos de IDs
        if(!seCompleto){
            sem_wait(mAccederSHM);
            if(id_inicial >= config->registros && shm_base->count == 0){
                // verficar si hay huecos en ids_generados
                if(0 != encontrar_hueco_id(ids_generados, config->registros, &shm_base->id_desde, &shm_base->id_hasta)){
                    // no hay huecos, se completo la generacion de productos
                    shm_base->se_completo = 1;
                    seCompleto = 1;
                    printf("Coordinador: se completó la generación de productos.\n");
                }
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
            ids_generados[prod.id] = 1; // marcar ID como generado
            sem_post(mAccederSHM);
            sem_post(capBuffer);
            coordinador_escribir_csv(archivo_salida, &prod);
            productos_generados++;
        }
        // usleep(1000);
    }
    free(ids_generados);
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