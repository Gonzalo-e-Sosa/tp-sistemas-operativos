#ifndef FILES_H
#define FILES_H

#include <stddef.h>

#include <stdio.h>

// Función genérica para escribir una fila en CSV
void write_csv_row(FILE *f, const char **fields, int num_fields);

// Función genérica para escribir un archivo CSV completo
void write_csv_file(const char *filename, const char ***rows, int num_rows, int num_fields, const char **header);

#endif // FILES_H
