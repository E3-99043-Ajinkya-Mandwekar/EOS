#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>

#define SHM_KEY 0x2345
#define SEM_KEY 0x2346
#define SIZE 5
#define ITEMS 20

enum { EMPTY = 0, FULL = 1, MUTEX = 2 };

struct shared_queue {
    int data[SIZE];
    int front;
    int rear;
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

static int sem_change(int semid, unsigned short num, short op)
{
    struct sembuf sb = { .sem_num = num, .sem_op = op, .sem_flg = 0 };
    return semop(semid, &sb, 1);
}

int main(void)
{
    int shmid = shmget(SHM_KEY, sizeof(struct shared_queue),
                       IPC_CREAT | 0600);
    if (shmid == -1) { perror("shmget"); return 1; }

    struct shared_queue *q = shmat(shmid, NULL, 0);
    if (q == (void *)-1) { perror("shmat"); shmctl(shmid, IPC_RMID, NULL); return 1; }

    int semid = semget(SEM_KEY, 3, IPC_CREAT | 0600);
    if (semid == -1) {
        perror("semget");
        shmdt(q); shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    union semun arg;
    unsigned short initial[3] = { SIZE, 0, 1 };
    arg.array = initial;
    if (semctl(semid, 0, SETALL, arg) == -1) {
        perror("semctl SETALL");
        shmdt(q); shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    q->front = 0;
    q->rear = 0;

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        shmdt(q); shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);
        return 1;
    }

    if (pid == 0) {
        /* Child consumes exactly ITEMS numbers. */
        for (int i = 0; i < ITEMS; i++) {
            if (sem_change(semid, FULL, -1) == -1 ||
                sem_change(semid, MUTEX, -1) == -1) {
                perror("child semop"); _exit(1);
            }

            int value = q->data[q->front];
            q->front = (q->front + 1) % SIZE;

            if (sem_change(semid, MUTEX, 1) == -1 ||
                sem_change(semid, EMPTY, 1) == -1) {
                perror("child semop"); _exit(1);
            }

            printf("Child removed: %d\n", value);
            fflush(stdout);
            usleep(100000);
        }
        shmdt(q);
        _exit(0);
    }

    /* Parent produces ITEMS random integers. */
    srand((unsigned int)(time(NULL) ^ getpid()));
    for (int i = 0; i < ITEMS; i++) {
        int value = rand() % 100;

        if (sem_change(semid, EMPTY, -1) == -1 ||
            sem_change(semid, MUTEX, -1) == -1) {
            perror("parent semop"); break;
        }

        q->data[q->rear] = value;
        q->rear = (q->rear + 1) % SIZE;

        if (sem_change(semid, MUTEX, 1) == -1 ||
            sem_change(semid, FULL, 1) == -1) {
            perror("parent semop"); break;
        }

        printf("Parent added: %d\n", value);
        fflush(stdout);
        usleep(50000);
    }

    waitpid(pid, NULL, 0);
    shmdt(q);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    return 0;
}
