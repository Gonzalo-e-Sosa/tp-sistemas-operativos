#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "control_de_parametros.h"
#include "utils.h"

#define DEFAULT_NUM_THREADS 5
#define DEFAULT_TOTAL_RECORDS 100

#define SHM_NAME "/shm_productos_tp"
#define SHM_UNLINK 1

int NUM_THREADS = DEFAULT_NUM_THREADS;
int TOTAL_RECORDS = DEFAULT_TOTAL_RECORDS;
int RECORDS_PER_THREAD = 20;
// #define NUM_THREADS 5                                 // TODO: utilizar parametros de entrada para generar N procesos generadores
// #define TOTAL_RECORDS (NUM_THREADS * RECORDS_PER_THREAD) // TODO: a modificarse por parametro
// #define RECORDS_PER_THREAD 20                            // TODO: a modificarse ya que es una division entre los parametros de entrada (Total de registros / Cantidad de procesos)
#define PRODUCT_NAME_COUNT 5

// TODO: a verse si se redondea la cantidad de registros a generar por cada proceso

// TODO: definir si el proceso coordinador es o no un hilo

const char *nombres[PRODUCT_NAME_COUNT] = {
    "Paracetamol 500mg",
    "Ibuprofeno 400mg",
    "Alcohol en gel 250ml",
    "Jeringa 5ml",
    "Guantes de látex M"};

typedef struct
{
    Producto *productos;
    int cantidad;
    int thread_idx;
    Producto *shm_base; // puntero base a la SHM
    int shm_offset;     // offset de inicio en la SHM
} ThreadData;

// Control de IDs
int next_id = 1;
pthread_mutex_t id_mutex = PTHREAD_MUTEX_INITIALIZER;

// Función para obtener el próximo ID (fácil de migrar a SHM)
int obtener_id()
{
    pthread_mutex_lock(&id_mutex);
    int id = next_id++;
    pthread_mutex_unlock(&id_mutex);
    return id;
}

// Genera un producto con datos aleatorios y un ID válido
Producto generar_producto()
{
    Producto prod;
    prod.id = obtener_id();
    snprintf(prod.codigo, sizeof(prod.codigo), "P%03d", prod.id);
    strncpy(prod.nombre, nombres[rand() % PRODUCT_NAME_COUNT], sizeof(prod.nombre) - 1);
    prod.nombre[sizeof(prod.nombre) - 1] = '\0';
    generate_lote(prod.lote, MAX_LOTE);
    generate_date(prod.fecha_ingreso, FECHA_INGRESO_MIN, FECHA_INGRESO_MAX);
    add_years(prod.fecha_ingreso, 1 + rand() % 3, prod.fecha_vencimiento);
    prod.cantidad = CANTIDAD_MIN + rand() % (CANTIDAD_MAX - CANTIDAD_MIN + 1);
    return prod;
}

// Simula el envío de un producto (fácil de migrar a SHM)
void enviar_producto(Producto *dest, Producto prod)
{
    *dest = prod;
}

void *generador(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    data->productos = malloc(sizeof(Producto) * data->cantidad);
    if (!data->productos)
    {
        fprintf(stderr, "Error: no se pudo asignar memoria para productos del hilo %d\n", data->thread_idx);
        pthread_exit(NULL);
    }
    for (int i = 0; i < data->cantidad; ++i)
    {
        Producto prod = generar_producto();
        enviar_producto(&data->productos[i], prod);
    }
    pthread_exit(NULL);
}

void print_producto(FILE *f, const Producto *p)
{
    fprintf(f, "%d,%s,%s,%s,%s,%s,%d\n",
            p->id,
            p->codigo,
            p->nombre,
            p->lote,
            p->fecha_ingreso,
            p->fecha_vencimiento,
            p->cantidad);
}

// Simula la recepción y escritura de productos (fácil de migrar a SHM)
void coordinador_escribir_csv(const char *filename, ThreadData *thread_data)
{
    FILE *f = fopen(filename, "w");
    if (!f)
    {
        perror("fopen");
        exit(1);
    }
    fprintf(f, "ID,Codigo,Descripcion,Lote,FechaIngreso,FechaVencimiento,Cantidad\n");
    for (int t = 0; t < NUM_THREADS; ++t)
    {
        for (int i = 0; i < thread_data[t].cantidad; ++i)
        {
            print_producto(f, &thread_data[t].productos[i]);
        }
        free(thread_data[t].productos);
    }
    fclose(f);
}

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
    
    RECORDS_PER_THREAD = (config.registros + config.generadores - 1) / config.generadores; // redondea hacia arriba
    TOTAL_RECORDS = RECORDS_PER_THREAD * config.generadores;                     // asegura múltiplo

    int shm_fd;
    size_t shm_size = sizeof(Producto) * TOTAL_RECORDS;
    Producto *shm_base = shm_create_and_map(SHM_NAME, shm_size, &shm_fd);
    
    srand(time(NULL));
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i)
    {
        thread_data[i].cantidad = RECORDS_PER_THREAD;
        thread_data[i].thread_idx = i;
        thread_data[i].shm_base = shm_base;
        thread_data[i].shm_offset = i * RECORDS_PER_THREAD;
        pthread_create(&threads[i], NULL, generador, &thread_data[i]);
    }
    for (int i = 0; i < NUM_THREADS; ++i)
    {    
        pthread_join(threads[i], NULL);
    }

    coordinador_escribir_csv(archivo_salida, thread_data);

    shm_unmap_and_close(shm_base, shm_size, shm_fd, SHM_NAME, SHM_UNLINK);

    printf("Generación de datos finalizada. Archivo: %s\n", archivo_salida);
    // Liberar memoria si se usó string
    if (config.archivo_salida) {
        free(config.archivo_salida);
    }
    return 0;
}
