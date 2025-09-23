#include "../include/index.h"
#include <pthread.h>

#define DEFAULT_NUM_THREADS 5
#define DEFAULT_TOTAL_RECORDS 100

#define SHM_NAME "/shm_productos_tp"
#define SHM_UNLINK 1

int NUM_THREADS = DEFAULT_NUM_THREADS;
int TOTAL_RECORDS = DEFAULT_TOTAL_RECORDS;
int RECORDS_PER_THREAD = 20;

typedef struct
{
    Producto *productos;
    int cantidad;
    int thread_idx;
    Producto *shm_base; // puntero base a la SHM
    int shm_offset;     // offset de inicio en la SHM
} ThreadData;

pthread_mutex_t id_mutex = PTHREAD_MUTEX_INITIALIZER;

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

int main(int argc, char *argv[])
{
    // Uso: ./main [num_generadores] [total_registros]
    if (argc > 1)
    {
        char *endptr = NULL;
        long val = strtol(argv[1], &endptr, 10);
        if (endptr == argv[1] || *endptr != '\0' || val <= 0)
            NUM_THREADS = DEFAULT_NUM_THREADS;
        else
            NUM_THREADS = (int)val;
    }
    if (argc > 2)
    {
        char *endptr = NULL;
        long val = strtol(argv[2], &endptr, 10);
        if (endptr == argv[2] || *endptr != '\0' || val <= 0)
            TOTAL_RECORDS = DEFAULT_TOTAL_RECORDS;
        else
            TOTAL_RECORDS = (int)val;
    }
    RECORDS_PER_THREAD = (TOTAL_RECORDS + NUM_THREADS - 1) / NUM_THREADS; // redondea hacia arriba
    TOTAL_RECORDS = RECORDS_PER_THREAD * NUM_THREADS;                     // asegura múltiplo

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

    // Unir todos los productos generados en un solo array
    Producto *all_productos = malloc(sizeof(Producto) * TOTAL_RECORDS);
    int idx = 0;
    for (int t = 0; t < NUM_THREADS; ++t)
    {
        for (int i = 0; i < thread_data[t].cantidad; ++i)
        {
            all_productos[idx++] = thread_data[t].productos[i];
        }
        free(thread_data[t].productos);
    }
    productos_escribir_csv("mock_stock.csv", all_productos, TOTAL_RECORDS);
    free(all_productos);

    shm_unmap_and_close(shm_base, shm_size, shm_fd, SHM_NAME, SHM_UNLINK);

    printf("Generación de datos finalizada. Archivo: mock_stock.csv\n");
    return 0;
}
