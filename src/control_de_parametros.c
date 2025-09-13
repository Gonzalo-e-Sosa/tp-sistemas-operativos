#include "control_de_parametros.h"
// Array global de parámetros disponibles
Parametro parametros[] = {
    {"--generadores", "Número de generadores (default: 1)", NULL, "int", 0},
    {"--registros",   "Número de registros (default: 100)", NULL, "int", 0},
    {"--output",      "Archivo de salida (default: registros.txt)", NULL, "string", 0},
    {"--help",        "Mostrar ayuda", NULL, "bool", 0},
    {NULL, NULL, NULL, NULL, 0} // Marcador de fin
};

void mostrar_ayuda(const char *nombre_programa) {
    printf("Uso: %s [OPCIONES]\n", nombre_programa);
    printf("Opciones:\n");
    
    for (int i = 0; parametros[i].nombre != NULL; i++) {
        printf("  %-15s %s\n", parametros[i].nombre, parametros[i].descripcion);
    }
}

// Función para inicializar los punteros de los parámetros
void inicializar_parametros(Configuracion *config) {
    for (int i = 0; parametros[i].nombre != NULL; i++) {
        if (strcmp(parametros[i].nombre, "--generadores") == 0) {
            parametros[i].variable = &config->generadores;
        }
        else if (strcmp(parametros[i].nombre, "--registros") == 0) {
            parametros[i].variable = &config->registros;
        }
        else if (strcmp(parametros[i].nombre, "--output") == 0) {
            parametros[i].variable = &config->archivo_salida;
        }
        else if (strcmp(parametros[i].nombre, "--help") == 0) {
            parametros[i].variable = &config->help;
        }
    }
}

// Función para buscar un parámetro por nombre
Parametro* buscar_parametro(const char *nombre) {
    for (int i = 0; parametros[i].nombre != NULL; i++) {
        if (strcmp(parametros[i].nombre, nombre) == 0) {
            return &parametros[i];
        }
    }
    return NULL;
}

// Función para procesar el valor según el tipo
char procesar_valor(Parametro *param, const char *valor) {
    if (strcmp(param->tipo, "int") == 0) {
        int *var_int = (int*)param->variable;
        *var_int = atoi(valor);
        if (*var_int <= 0) {
            fprintf(stderr, "Error: %s debe ser un número positivo\n", param->nombre);
            return 0;
        }
    }
    else if (strcmp(param->tipo, "string") == 0) {
        char **var_str = (char**)param->variable;
        *var_str = strdup(valor); // Necesitarías free() luego
    }
    else if (strcmp(param->tipo, "bool") == 0) {
        char *var_bool = (char*)param->variable;
        *var_bool = 1;
    }
    return 1;
}

char parsear_parametros(int argc, char *argv[], Configuracion *config) {
    // Valores por defecto
    config->generadores = NUM_GENERATOR;
    config->registros = NUM_REGISTER;
    config->archivo_salida = NULL;
    config->help = 0;
    
    // Inicializar punteros
    inicializar_parametros(config);

    for (int i = 1; i < argc; i++) {
        Parametro *param = buscar_parametro(argv[i]);
        
        if (param == NULL) {
            fprintf(stderr, "Error: Opción desconocida '%s'\n", argv[i]);
            return 0;
        }
        
        if (strcmp(param->tipo, "bool") == 0) {
            // Para booleanos, no necesita valor
            if (!procesar_valor(param, "")) return 0;
        } else {
            // Para otros tipos, necesita valor
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: %s requiere un valor\n", param->nombre);
                return 0;
            }
            if (!procesar_valor(param, argv[i + 1])) return 0;
            i++; // Saltar el valor
        }
    }
    
    return 1;
}