#include "utils.h"

#define PRODUCT_NAME_COUNT 5

const char *nombres[PRODUCT_NAME_COUNT] = {
    "Paracetamol 500mg",
    "Ibuprofeno 400mg",
    "Alcohol en gel 250ml",
    "Jeringa 5ml",
    "Guantes de látex M"};

static int next_id = 1;

int obtener_id()
{
    return next_id++;
}

Producto generar_producto()
{
    Producto prod;
    prod.id = obtener_id();
    snprintf(prod.codigo, sizeof(prod.codigo), "P%03d", prod.id);
    strncpy(prod.nombre, nombres[rand() % PRODUCT_NAME_COUNT], sizeof(prod.nombre) - 1);
    prod.nombre[sizeof(prod.nombre) - 1] = '\0';
    generate_lote(prod.lote, 8);
    generate_date(prod.fecha_ingreso, "01-01-2024", "31-12-2025");
    add_years(prod.fecha_ingreso, 1 + rand() % 3, prod.fecha_vencimiento);
    prod.cantidad = 10 + rand() % 491;
    return prod;
}
