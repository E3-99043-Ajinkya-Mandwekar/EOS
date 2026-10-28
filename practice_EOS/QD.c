#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>  // Include for exit and rand
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>  // Include for mmap

#define SHARED_MEMORY_NAME "/my_shared_memory"
#define SHARED_MEMORY_SIZE (1024 * 1024)  // 1 MB

void generate_numbers() {
    int fd = shm_open(SHARED_MEMORY_NAME, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    int *shared_memory = (int *)mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE,
                                      MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < SHARED_MEMORY_SIZE / sizeof(int); ++i) {
        shared_memory[i] = rand() % 100;
    }

    munmap(shared_memory, SHARED_MEMORY_SIZE);
    close(fd);
}

void process_results() {
    int fd = shm_open(SHARED_MEMORY_NAME, O_RDONLY, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    int *shared_memory = (int *)mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < SHARED_MEMORY_SIZE / sizeof(int); ++i) {
        printf("%d ", shared_memory[i]);
    }
    printf("\n");

    munmap(shared_memory, SHARED_MEMORY_SIZE);
    close(fd);
}

int main() {
    generate_numbers();
    process_results();
    return 0;
}

