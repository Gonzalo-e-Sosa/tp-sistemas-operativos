#include "utils.h"

// Genera un string de lote aleatorio
void generar_lote(char *dest, size_t len)
{
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (size_t i = 0; i < len - 1; ++i)
    {
        dest[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    dest[len - 1] = '\0';
}

// Genera una fecha aleatoria entre dos fechas
void generar_fecha(char *dest, const char *inicio, const char *fin)
{
    struct tm tm_ini = {0}, tm_fin = {0};
    strptime(inicio, FECHA_FORMATO, &tm_ini);
    strptime(fin, FECHA_FORMATO, &tm_fin);
    time_t t_ini = mktime(&tm_ini);
    time_t t_fin = mktime(&tm_fin);
    time_t t_rand = t_ini + rand() % (t_fin - t_ini);
    struct tm *tm_rand = localtime(&t_rand);
    strftime(dest, 11, FECHA_FORMATO, tm_rand);
}

// Suma años a una fecha
void sumar_anios(const char *fecha, int anos, char *dest)
{
    struct tm tm_fecha = {0};
    strptime(fecha, FECHA_FORMATO, &tm_fecha);
    tm_fecha.tm_year += anos;
    mktime(&tm_fecha);
    strftime(dest, 11, FECHA_FORMATO, &tm_fecha);
}
