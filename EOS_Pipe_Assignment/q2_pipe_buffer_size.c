#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
    int fd[2];

    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    int size = fcntl(fd[0], F_GETPIPE_SZ);

    if (size == -1) {
        perror("fcntl");
        close(fd[0]);
        close(fd[1]);
        return 1;
    }

    printf("Pipe buffer capacity = %d bytes\n", size);

    long atomic_size = fpathconf(fd[0], _PC_PIPE_BUF);
    if (atomic_size != -1)
        printf("PIPE_BUF atomic-write limit = %ld bytes\n",
               atomic_size);

    close(fd[0]);
    close(fd[1]);
    return 0;
}
