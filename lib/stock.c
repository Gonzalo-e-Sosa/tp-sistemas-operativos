#include "../include/files.h"
#include "../include/stock.h"

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

// Escribe un lote de productos en un archivo CSV usando el módulo genérico
void producto_to_fields(const Producto *p, const char **fields)
{
    static char id[12], cantidad[12];
    snprintf(id, sizeof(id), "%d", p->id);
    snprintf(cantidad, sizeof(cantidad), "%d", p->cantidad);
    fields[0] = id;
    fields[1] = p->codigo;
    fields[2] = p->nombre;
    fields[3] = p->lote;
    fields[4] = p->fecha_ingreso;
    fields[5] = p->fecha_vencimiento;
    fields[6] = cantidad;
}

void productos_escribir_csv(const char *filename, Producto *productos, int num_productos)
{
    const char *header[] = {"ID", "Codigo", "Descripcion", "Lote", "FechaIngreso", "FechaVencimiento", "Cantidad"};
    int num_fields = 7;
    const char ***rows = malloc(sizeof(char **) * num_productos);
    char **id_bufs = malloc(sizeof(char *) * num_productos);
    char **cantidad_bufs = malloc(sizeof(char *) * num_productos);
    if (!rows || !id_bufs || !cantidad_bufs)
    {
        printf("[ERROR] malloc failed for rows or buffers\n");
        exit(1);
    }
    for (int i = 0; i < num_productos; ++i)
    {
        id_bufs[i] = malloc(12);
        cantidad_bufs[i] = malloc(12);
        if (!id_bufs[i] || !cantidad_bufs[i])
        {
            printf("[ERROR] malloc failed for product %d\n", i);
            exit(1);
        }
        snprintf(id_bufs[i], 12, "%d", productos[i].id);
        snprintf(cantidad_bufs[i], 12, "%d", productos[i].cantidad);
        rows[i] = malloc(sizeof(char *) * num_fields);
        if (!rows[i])
        {
            printf("[ERROR] malloc failed for row %d\n", i);
            exit(1);
        }
        rows[i][0] = id_bufs[i];
        rows[i][1] = productos[i].codigo;
        rows[i][2] = productos[i].nombre;
        rows[i][3] = productos[i].lote;
        rows[i][4] = productos[i].fecha_ingreso;
        rows[i][5] = productos[i].fecha_vencimiento;
        rows[i][6] = cantidad_bufs[i];
        // DEBUG: printf("[DEBUG] Producto %d: id=%s, codigo=%s\n", i, id_bufs[i], productos[i].codigo);
    }
    // DEBUG: printf("[DEBUG] Llamando a write_csv_file...\n");
    write_csv_file(filename, rows, num_productos, num_fields, header);
    // DEBUG: printf("[DEBUG] CSV escrito. Liberando buffers...\n");
    for (int i = 0; i < num_productos; ++i)
    {
        free(id_bufs[i]);
        free(cantidad_bufs[i]);
        free(rows[i]);
    }
    free(id_bufs);
    free(cantidad_bufs);
    free(rows);
    // DEBUG: printf("[DEBUG] productos_escribir_csv finalizado.\n");
}