#ifndef DATE_H
#define DATE_H

#define _XOPEN_SOURCE
#include <time.h>

// Suma años a una fecha
void add_years(const char *date, int years, char *dest);

#endif // DATE_H
