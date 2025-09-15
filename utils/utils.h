#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Suma años a una fecha
void add_years(const char *date, int years, char *dest);

// Genera una fecha aleatoria entre dos fechas dadas
void generate_date(char *dest, const char *start, const char *end);

// Genera un lote aleatorio
void generate_lote(char *dest, size_t len);

#endif // UTILS_H
