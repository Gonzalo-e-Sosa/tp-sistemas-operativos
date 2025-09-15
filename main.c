#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "utils/utils.h"
#include <pthread.h>

#define NUM_THREADS 5                                    // TODO: utilizar parametros de entrada para generar N procesos generadores
#define TOTAL_RECORDS (NUM_THREADS * RECORDS_PER_THREAD) // TODO: a modificarse por parametro
#define RECORDS_PER_THREAD 20                            // TODO: a modificarse ya que es una division entre los parametros de entrada (Total de registros / Cantidad de procesos)
#define PRODUCT_NAME_COUNT 5

// TODO: a verse si se redondea la cantidad de registros a generar por cada proceso

// TODO: definir si el proceso coordinador es o no un hilo

typedef struct
{
    int id;
    char codigo[8];
    char nombre[64];
    char lote[16];
    char fecha_ingreso[11];
    char fecha_vencimiento[11];
    int cantidad;
} Producto;

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
        generate_lote(p->lote, 8);
        generate_date(p->fecha_ingreso, "01-01-2024", "31-12-2025");
        add_years(p->fecha_ingreso, 1 + rand() % 3, p->fecha_vencimiento);
        p->cantidad = 10 + rand() % 491;
    }
    return NULL;
}

void print_producto(FILE *f, int i)
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

// Es main el proceso coordinador???
// TODO: crear una funcion coordinador
int main(int argc, char *argv[])
{
    // TODO: utilizar parametros de entrada para generar N procesos generadores

    // printf("Argumentos: ");
    // for (int i = 0; i < argc; i++)
    // {
    //     printf("%s ", argv[i]);
    // }
    // printf("\n");

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

    write_csv(
        "mock_stock.csv",
        "ID,Codigo,Descripcion,Lote,FechaIngreso,FechaVencimiento,Cantidad",
        print_producto,
        TOTAL_RECORDS);

    printf("Generación de datos finalizada. Archivo: mock_stock.csv\n");
    return 0;
}
