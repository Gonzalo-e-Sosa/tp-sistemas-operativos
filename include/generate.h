#ifndef GENERATE_H
#define GENERATE_H

#include <stddef.h>

#include <stdio.h>
#include <stdlib.h>
#include "date.h"

// Genera un lote aleatorio
void generate_lote(char *dest, size_t len);

// Genera una fecha aleatoria entre dos fechas dadas
void generate_date(char *dest, const char *start, const char *end);

#endif // GENERATE_H
