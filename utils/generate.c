#include "utils.h"

void generate_lote(char *dest, size_t len)
{
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (size_t i = 0; i < len - 1; ++i)
    {
        dest[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    dest[len - 1] = '\0';
}

void generate_date(char *dest, const char *start, const char *end)
{
    struct tm tm_start = {0}, tm_end = {0};
    strptime(start, "%d-%m-%Y", &tm_start);
    strptime(end, "%d-%m-%Y", &tm_end);
    time_t t_start = mktime(&tm_start);
    time_t t_end = mktime(&tm_end);
    time_t t_rand = t_start + rand() % (t_end - t_start);
    struct tm *tm_rand = localtime(&t_rand);
    strftime(dest, 11, "%d-%m-%Y", tm_rand);
}
