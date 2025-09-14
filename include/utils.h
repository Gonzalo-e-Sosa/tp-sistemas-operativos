#ifndef UTILS_H
#define UTILS_H

#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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


// Utilidades generales
void generar_lote(char *dest, size_t len);
// Genera una fecha aleatoria entre dos fechas
void generar_fecha(char *dest, const char *inicio, const char *fin);
// Suma años a una fecha
void sumar_anios(const char *fecha, int anos, char *dest);
#endif // UTILS_H