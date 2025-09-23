#include <stdlib.h>
#include "../include/files.h"

void write_csv_row(FILE *f, const char **fields, int num_fields)
{
    for (int i = 0; i < num_fields; ++i)
    {
        fprintf(f, "%s%s", fields[i], (i < num_fields - 1) ? "," : "\n");
    }
}

void write_csv_file(const char *filename, const char ***rows, int num_rows, int num_fields, const char **header)
{
    FILE *f = fopen(filename, "w");
    if (!f)
    {
        perror("fopen");
        exit(1);
    }
    // Escribir encabezado
    write_csv_row(f, header, num_fields);
    // Escribir filas
    for (int i = 0; i < num_rows; ++i)
    {
        write_csv_row(f, rows[i], num_fields);
    }
    fclose(f);
}
