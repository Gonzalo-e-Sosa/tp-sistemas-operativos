#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stddef.h>

// Suma años a una fecha
void add_years(const char *date, int years, char *dest);

// Genera una fecha aleatoria entre dos fechas dadas
void generate_date(char *dest, const char *start, const char *end);

// Genera un lote aleatorio
void generate_lote(char *dest, size_t len);

// Stock

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

// POSIX

// Crea o abre una SHM POSIX y la mapea en memoria
void *shm_create_and_map(const char *name, size_t size, int *fd_out);
// Desmapea y cierra la SHM
void shm_unmap_and_close(void *addr, size_t size, int fd, const char *name, int unlink_flag);

#endif // UTILS_H
