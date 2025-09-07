#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 5                                 // TODO: utilizar parametros de entrada para generar N procesos generadores
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

// Genera un string de lote aleatorio
void generar_lote(char *dest, size_t len)
{
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (size_t i = 0; i < len - 1; ++i)
    {
        dest[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    dest[len - 1] = '\0';
}

// Genera una fecha aleatoria entre dos fechas
void generar_fecha(char *dest, const char *inicio, const char *fin)
{
    struct tm tm_ini = {0}, tm_fin = {0};
    strptime(inicio, "%d-%m-%Y", &tm_ini);
    strptime(fin, "%d-%m-%Y", &tm_fin);
    time_t t_ini = mktime(&tm_ini);
    time_t t_fin = mktime(&tm_fin);
    time_t t_rand = t_ini + rand() % (t_fin - t_ini);
    struct tm *tm_rand = localtime(&t_rand);
    strftime(dest, 11, "%d-%m-%Y", tm_rand);
}

// Suma años a una fecha
void sumar_anios(const char *fecha, int anos, char *dest)
{
    struct tm tm_fecha = {0};
    strptime(fecha, "%d-%m-%Y", &tm_fecha);
    tm_fecha.tm_year += anos;
    mktime(&tm_fecha);
    strftime(dest, 11, "%d-%m-%Y", &tm_fecha);
}

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
        generar_lote(p->lote, 8);
        generar_fecha(p->fecha_ingreso, "01-01-2024", "31-12-2025");
        sumar_anios(p->fecha_ingreso, 1 + rand() % 3, p->fecha_vencimiento);
        p->cantidad = 10 + rand() % 491;
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
    escribir_csv("mock_stock.csv");
    printf("Generación de datos finalizada. Archivo: mock_stock.csv\n");
    return 0;
}
