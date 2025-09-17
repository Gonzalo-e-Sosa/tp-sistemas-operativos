#ifndef CONTROL_DE_PARAMETROS_H
#define CONTROL_DE_PARAMETROS_H

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#define HELP 100
#define NUM_REGISTER 100
#define NUM_GENERATOR 5

#define DEFAULT_OUTPUT_FILE "mock_stock.csv"
// Estructura para definir cada parámetro
typedef struct {
    const char *nombre;      // Nombre del parámetro (ej: "--generadores")
    const char *descripcion; // Descripción para help
    void *variable;          // Puntero a la variable donde guardar el valor
    const char *tipo;        // Tipo de dato: "int", "string", "char"
    char requerido;          // Si es requerido o opcional
} Parametro;

// Estructura de configuración
typedef struct {
    int generadores;
    int registros;
    char *archivo_salida;
    char help;
} Configuracion;

void mostrar_ayuda(const char *nombre_programa);
void inicializar_parametros(Configuracion *config);
// Función para buscar un parámetro por nombre
Parametro* buscar_parametro(const char *nombre);
// Función para procesar el valor según el tipo
char procesar_valor(Parametro *param, const char *valor);
char parsear_parametros(int argc, char *argv[], Configuracion *config);
#endif