#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "control_de_parametros.h"
#include "utils.h"
#define NUM_THREADS 5                                 // TODO: utilizar parametros de entrada para generar N procesos generadores
#define TOTAL_RECORDS (NUM_THREADS * RECORDS_PER_THREAD) // TODO: a modificarse por parametro
#define RECORDS_PER_THREAD 20                            // TODO: a modificarse ya que es una division entre los parametros de entrada (Total de registros / Cantidad de procesos)
#define PRODUCT_NAME_COUNT 5

// TODO: a verse si se redondea la cantidad de registros a generar por cada proceso

// TODO: definir si el proceso coordinador es o no un hilo

const char *nombres[PRODUCT_NAME_COUNT] = {
    "Paracetamol 500mg",
    "Ibuprofeno 400mg",
    "Alcohol en gel 250ml",
    "Jeringa 5ml",
    "Guantes de látex M"};

Producto productos[TOTAL_RECORDS];
int next_id = 1;
pthread_mutex_t id_mutex = PTHREAD_MUTEX_INITIALIZER;

// TODO: A Modificarse IMPORTANTE
void *generador(void *arg)
{
    // TODO: a modificarse para que el id sea generador por el proceso coordinador y NO por INDICE
    int thread_idx = *(int *)arg;
    int base = thread_idx * RECORDS_PER_THREAD;
    for (int i = 0; i < RECORDS_PER_THREAD; ++i)
    {
        Producto *p = &productos[base + i];
        pthread_mutex_lock(&id_mutex);
        p->id = next_id++;
        pthread_mutex_unlock(&id_mutex);
        snprintf(p->codigo, sizeof(p->codigo), "P%03d", p->id);
        strncpy(p->nombre, nombres[rand() % PRODUCT_NAME_COUNT], sizeof(p->nombre));
        generar_lote(p->lote, MAX_CODIGO);
        generar_fecha(p->fecha_ingreso, FECHA_INGRESO_MIN, FECHA_INGRESO_MAX);
        sumar_anios(p->fecha_ingreso, 1 + rand() % 3, p->fecha_vencimiento);
        p->cantidad = CANTIDAD_MIN + rand() % CANTIDAD_MAX;
    }
    return NULL;
}

void escribir_csv(const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (!f)
    {
        perror("fopen");
        exit(1);
    }
    fprintf(f, "ID,Codigo,Descripcion,Lote,FechaIngreso,FechaVencimiento,Cantidad\n");
    for (int i = 0; i < TOTAL_RECORDS; ++i)
    {
        fprintf(f, "%d,%s,%s,%s,%s,%s,%d\n",
                productos[i].id,
                productos[i].codigo,
                productos[i].nombre,
                productos[i].lote,
                productos[i].fecha_ingreso,
                productos[i].fecha_vencimiento,
                productos[i].cantidad);
    }
    fclose(f);
}

// Es main el proceso coordinador???
// TODO: crear una funcion coordinador

int main(int argc, char *argv[])
{
    // utilizar parametros de entrada para generar N procesos generadores
    Configuracion config;
    char* archivo_salida = NULL;
    if(!parsear_parametros(argc, argv, &config) || config.help){
        mostrar_ayuda(argv[0]);
        return HELP;
    }
    printf("Configuración:\n");
    printf("  Generadores: %d\n", config.generadores);
    printf("  Registros: %d\n", config.registros);
    archivo_salida = config.archivo_salida ? config.archivo_salida : DEFAULT_OUTPUT_FILE;
    printf("  Archivo salida: %s\n", archivo_salida);
    
    
    srand(time(NULL));
    pthread_t threads[NUM_THREADS];
    int idx[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        idx[i] = i;
        pthread_create(&threads[i], NULL, generador, &idx[i]);
    }
    for (int i = 0; i < NUM_THREADS; ++i)
    {    
        pthread_join(threads[i], NULL);
    }
    escribir_csv(archivo_salida);
    printf("Generación de datos finalizada. Archivo: %s\n", archivo_salida);

    // Liberar memoria si se usó string
    if (config.archivo_salida) {
        free(config.archivo_salida);
    }
    return 0;
}
