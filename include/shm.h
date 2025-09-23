#ifndef SHM_H
#define SHM_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Crea o abre una SHM POSIX y la mapea en memoria
void *shm_create_and_map(const char *name, size_t size, int *fd_out);
// Desmapea y cierra la SHM
void shm_unmap_and_close(void *addr, size_t size, int fd, const char *name, int unlink_flag);

#endif // SHM_H
