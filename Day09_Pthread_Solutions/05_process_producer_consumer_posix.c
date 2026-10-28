#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define SHM_NAME "/day09_pc_shared_memory"
#define BUFFER_SIZE 5
#define ITEMS 10

struct shared_data {
    pthread_mutex_t mutex;
    sem_t empty_slots;
    sem_t full_slots;
    int buffer[BUFFER_SIZE];
    int in;
    int out;
};

static int shm_fd = -1;
static struct shared_data *shared = MAP_FAILED;
static volatile sig_atomic_t interrupted = 0;

static void handle_signal(int signo)
{
    (void)signo;
    interrupted = 1;
}

/* This is a small teaching example. Ctrl+C stops the parent; the parent
   waits for its child and removes the named shared-memory object. */
int main(void)
{
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    if (ftruncate(shm_fd, (off_t)sizeof(*shared)) == -1) {
        perror("ftruncate");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return 1;
    }

    shared = mmap(NULL, sizeof(*shared), PROT_READ | PROT_WRITE,
                  MAP_SHARED, shm_fd, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return 1;
    }

    pthread_mutexattr_t mattr;
    if (pthread_mutexattr_init(&mattr) != 0 ||
        pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED) != 0 ||
        pthread_mutex_init(&shared->mutex, &mattr) != 0) {
        fprintf(stderr, "Could not initialize process-shared mutex\n");
        munmap(shared, sizeof(*shared));
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return 1;
    }
    pthread_mutexattr_destroy(&mattr);

    /* pshared=1 makes these unnamed semaphores process-shared. */
    if (sem_init(&shared->empty_slots, 1, BUFFER_SIZE) == -1 ||
        sem_init(&shared->full_slots, 1, 0) == -1) {
        perror("sem_init");
        pthread_mutex_destroy(&shared->mutex);
        munmap(shared, sizeof(*shared));
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return 1;
    }

    shared->in = 0;
    shared->out = 0;

    pid_t child = fork();
    if (child == -1) {
        perror("fork");
        sem_destroy(&shared->empty_slots);
        sem_destroy(&shared->full_slots);
        pthread_mutex_destroy(&shared->mutex);
        munmap(shared, sizeof(*shared));
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return 1;
    }

    if (child == 0) {
        /* Child is the consumer. */
        for (int i = 0; i < ITEMS; i++) {
            while (sem_wait(&shared->full_slots) == -1 && errno == EINTR) {
                if (interrupted)
                    _exit(1);
            }

            pthread_mutex_lock(&shared->mutex);
            int value = shared->buffer[shared->out];
            shared->out = (shared->out + 1) % BUFFER_SIZE;
            pthread_mutex_unlock(&shared->mutex);

            sem_post(&shared->empty_slots);
            printf("Consumer received: %d\n", value);
            fflush(stdout);
        }
        _exit(0);
    }

    /* Parent is the producer. */
    for (int value = 1; value <= ITEMS && !interrupted; value++) {
        while (sem_wait(&shared->empty_slots) == -1) {
            if (errno == EINTR && !interrupted)
                continue;
            if (interrupted)
                break;
            perror("sem_wait");
            break;
        }
        if (interrupted)
            break;

        pthread_mutex_lock(&shared->mutex);
        shared->buffer[shared->in] = value;
        shared->in = (shared->in + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&shared->mutex);

        sem_post(&shared->full_slots);
        printf("Producer sent: %d\n", value);
        fflush(stdout);
        sleep(1);
    }

    if (interrupted) {
        /* Wake consumer if it is blocked so it can exit. */
        sem_post(&shared->full_slots);
    }

    int status;
    while (waitpid(child, &status, 0) == -1 && errno == EINTR) {
        if (interrupted)
            continue;
    }

    sem_destroy(&shared->empty_slots);
    sem_destroy(&shared->full_slots);
    pthread_mutex_destroy(&shared->mutex);
    munmap(shared, sizeof(*shared));
    close(shm_fd);
    shm_unlink(SHM_NAME);

    return 0;
}
