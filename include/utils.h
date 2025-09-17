#ifndef UTILS_H
#define UTILS_H

#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stddef.h>

// Rango de fechas para ingreso y vencimiento
#define FECHA_INGRESO_MIN "01-01-2024"
#define FECHA_INGRESO_MAX "31-12-2025"
#define FECHA_FORMATO "%d-%m-%Y"

// Rango de cantidad de stock
#define CANTIDAD_MIN 10
#define CANTIDAD_MAX 500

// Cantidad máxima de productos en la lista
#define MAX_PRODUCTOS 20
#define MAX_NOMBRE 64
#define MAX_CODIGO 8
#define MAX_LOTE 16
#define MAX_FECHA 12


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

// Suma años a una fecha
void add_years(const char *date, int years, char *dest);

// Genera una fecha aleatoria entre dos fechas dadas
void generate_date(char *dest, const char *start, const char *end);

// Genera un lote aleatorio
void generate_lote(char *dest, size_t len);

// POSIX

// Crea o abre una SHM POSIX y la mapea en memoria
void *shm_create_and_map(const char *name, size_t size, int *fd_out);
// Desmapea y cierra la SHM
void shm_unmap_and_close(void *addr, size_t size, int fd, const char *name, int unlink_flag);

#endif // UTILS_H