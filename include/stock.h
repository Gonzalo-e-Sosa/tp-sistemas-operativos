#ifndef STOCK_H
#define STOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generate.h"
#include "date.h"

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

Producto generar_producto();
void producto_to_fields(const Producto *p, const char **fields);
void productos_escribir_csv(const char *filename, Producto *productos, int num_productos);

#endif // STOCK_H
