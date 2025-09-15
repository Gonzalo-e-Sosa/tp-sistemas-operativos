#include "utils/utils.h"
#include <pthread.h>

#define DEFAULT_NUM_THREADS 5
#define DEFAULT_TOTAL_RECORDS 100

int NUM_THREADS = DEFAULT_NUM_THREADS;
int TOTAL_RECORDS = DEFAULT_TOTAL_RECORDS;
int RECORDS_PER_THREAD = 20;
#define PRODUCT_NAME_COUNT 5

typedef struct
{
    int id;
    char codigo[8];
    char nombre[64];
    char lote[16];
    char fecha_ingreso[11];     // dd-mm-yyyy
    char fecha_vencimiento[11]; // dd-mm-yyyy
    int cantidad;
} Producto;

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
} ThreadData;
int next_id = 1;
pthread_mutex_t id_mutex = PTHREAD_MUTEX_INITIALIZER;

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
        Producto *p = &data->productos[i];
        pthread_mutex_lock(&id_mutex);
        p->id = next_id++;
        pthread_mutex_unlock(&id_mutex);
        snprintf(p->codigo, sizeof(p->codigo), "P%03d", p->id);
        strncpy(p->nombre, nombres[rand() % PRODUCT_NAME_COUNT], sizeof(p->nombre) - 1);
        p->nombre[sizeof(p->nombre) - 1] = '\0';
        generate_lote(p->lote, 8);
        generate_date(p->fecha_ingreso, "01-01-2024", "31-12-2025");
        add_years(p->fecha_ingreso, 1 + rand() % 3, p->fecha_vencimiento);
        p->cantidad = 10 + rand() % 491;
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

void escribir_csv(const char *filename, ThreadData *thread_data)
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

// Es main el proceso coordinador???
// TODO: crear una funcion coordinador
int main(int argc, char *argv[])
{
    // Uso: ./main [num_generadores] [total_registros]
    if (argc > 1)
    {
        NUM_THREADS = atoi(argv[1]);
        if (NUM_THREADS <= 0)
            NUM_THREADS = DEFAULT_NUM_THREADS;
    }
    if (argc > 2)
    {
        TOTAL_RECORDS = atoi(argv[2]);
        if (TOTAL_RECORDS <= 0)
            TOTAL_RECORDS = DEFAULT_TOTAL_RECORDS;
    }
    RECORDS_PER_THREAD = (TOTAL_RECORDS + NUM_THREADS - 1) / NUM_THREADS; // redondea hacia arriba
    TOTAL_RECORDS = RECORDS_PER_THREAD * NUM_THREADS;                     // asegura múltiplo

    srand(time(NULL));
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i)
    {
        thread_data[i].cantidad = RECORDS_PER_THREAD;
        thread_data[i].thread_idx = i;
        thread_data[i].productos = NULL;
        pthread_create(&threads[i], NULL, generador, &thread_data[i]);
    }
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    escribir_csv("mock_stock.csv", thread_data);

    printf("Generación de datos finalizada. Archivo: mock_stock.csv\n");
    return 0;
}
