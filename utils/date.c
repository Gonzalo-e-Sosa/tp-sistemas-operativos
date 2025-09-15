#include "utils.h"

void add_years(const char *date, int years, char *dest)
{
    struct tm tm_date = {0};
    strptime(date, "%d-%m-%Y", &tm_date);
    tm_date.tm_year += years;
    mktime(&tm_date);
    strftime(dest, 11, "%d-%m-%Y", &tm_date);
}
