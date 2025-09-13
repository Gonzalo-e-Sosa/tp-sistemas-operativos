
#include "control_de_parametros.h"

void mostrar_ayuda(const char *nombre_programa) {
    printf("Uso: %s [OPCIONES]\n", nombre_programa);
    printf("Opciones:\n");
    printf("  --generadores NUM   Número de generadores (default: 1)\n");
    printf("  --registros NUM     Número de registros (default: 100)\n");
    printf("  --help              Muestra esta ayuda\n");
}

char parsear_parametros(int argc, char *argv[], Configuracion *config) {
    // Valores por defecto
    config->generadores = NUM_GENERATOR;
    config->registros = NUM_REGISTER;
    config->help = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            config->help = 1;
            return 1;
        }
        else if (strcmp(argv[i], "--generadores") == 0) {
            if (i + 1 < argc) {
                config->generadores = atoi(argv[i + 1]);
                if (config->generadores <= 0) {
                    fprintf(stderr, "Error: --generadores debe ser un número positivo\n");
                    return 0;
                }
                i++; // Saltar el siguiente argumento (el valor)
            } else {
                fprintf(stderr, "Error: --generadores requiere un valor\n");
                return 0;
            }
        }
        else if (strcmp(argv[i], "--registros") == 0) {
            if (i + 1 < argc) {
                config->registros = atoi(argv[i + 1]);
                if (config->registros <= 0) {
                    fprintf(stderr, "Error: --registros debe ser un número positivo\n");
                    return 0;
                }
                i++; // Saltar el siguiente argumento (el valor)
            } else {
                fprintf(stderr, "Error: --registros requiere un valor\n");
                return 0;
            }
        }
        else {
            fprintf(stderr, "Error: Opción desconocida '%s'\n", argv[i]);
            return 0;
        }
    }
    return 1;
}