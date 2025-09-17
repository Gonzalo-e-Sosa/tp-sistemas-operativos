#define _XOPEN_SOURCE
#include "utils.h"

void add_years(const char *date, int years, char *dest)
{
    struct tm tm_date = {0};
    strptime(date, FECHA_FORMATO, &tm_date);
    tm_date.tm_year += years;
    mktime(&tm_date);
    strftime(dest, 11, FECHA_FORMATO, &tm_date);
}
