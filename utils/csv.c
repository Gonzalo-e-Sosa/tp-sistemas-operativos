
#include "utils.h"

void write_csv(const char *filename, const char *header,
               void (*print_row)(FILE *, int), int limit)
{
    FILE *f = fopen(filename, "w");
    if (!f)
    {
        perror("fopen");
        exit(1);
    }
    fprintf(f, "%s\n", header);
    for (int i = 0; i < limit; ++i)
    {
        print_row(f, i);
    }
    fclose(f);
}
