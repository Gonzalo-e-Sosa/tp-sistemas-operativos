#include "utils.h"
// Control de IDs
// int next_id = 1;
// pthread_mutex_t id_mutex = PTHREAD_MUTEX_INITIALIZER;
const char *nombres[PRODUCT_NAME_COUNT] = {
    "Paracetamol 500mg",
    "Ibuprofeno 400mg",
    "Alcohol en gel 250ml",
    "Jeringa 5ml",
    "Guantes de látex M"};
// Función para obtener el próximo ID (fácil de migrar a SHM)
// int obtener_id()
// {
//     pthread_mutex_lock(&id_mutex);
//     int id = next_id++;
//     pthread_mutex_unlock(&id_mutex);
//     return id;
// }
// Genera un producto con datos aleatorios y un ID válido
Producto generar_producto(const int id)
{
    Producto prod;
    prod.id = id;
    //prod.id = obtener_id();
    snprintf(prod.codigo, sizeof(prod.codigo), "P%03d", prod.id);
    strncpy(prod.nombre, nombres[rand() % PRODUCT_NAME_COUNT], sizeof(prod.nombre) - 1);
    prod.nombre[sizeof(prod.nombre) - 1] = '\0';
    generate_lote(prod.lote, MAX_LOTE);
    generate_date(prod.fecha_ingreso, FECHA_INGRESO_MIN, FECHA_INGRESO_MAX);
    add_years(prod.fecha_ingreso, 1 + rand() % 3, prod.fecha_vencimiento);
    prod.cantidad = CANTIDAD_MIN + rand() % (CANTIDAD_MAX - CANTIDAD_MIN + 1);
    return prod;
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