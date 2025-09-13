#ifndef CONTROL_DE_PARAMETROS_H
#define CONTROL_DE_PARAMETROS_H

#include <stdio.h>
#include <string.h>

#define HELP 100
#define NUM_REGISTER 100
#define NUM_GENERATOR 5
typedef struct {
    int generadores;
    int registros;
    char help;
} Configuracion;

void mostrar_ayuda(const char *nombre_programa);
char parsear_parametros(int argc, char *argv[], Configuracion *config);
#endif