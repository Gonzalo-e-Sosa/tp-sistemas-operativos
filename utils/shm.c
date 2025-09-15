#include "utils.h"

void *shm_create_and_map(const char *name, size_t size, int *fd_out)
{
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (fd == -1)
    {
        perror("shm_open");
        exit(1);
    }
    if (ftruncate(fd, size) == -1)
    {
        perror("ftruncate");
        close(fd);
        exit(1);
    }
    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        exit(1);
    }
    if (fd_out)
        *fd_out = fd;
    return addr;
}

void shm_unmap_and_close(void *addr, size_t size, int fd, const char *name, int unlink_flag)
{
    if (munmap(addr, size) == -1)
    {
        perror("munmap");
    }
    if (close(fd) == -1)
    {
        perror("close");
    }
    if (unlink_flag && name)
    {
        if (shm_unlink(name) == -1)
        {
            perror("shm_unlink");
        }
    }
}
