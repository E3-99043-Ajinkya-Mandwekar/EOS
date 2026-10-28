
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define SHM_KEY 0x1234

int main(void)
{
    int shmid, i;
    int *ptr;

    shmid = shmget(SHM_KEY, sizeof(int) * 5, IPC_CREAT | 0600);

    if (shmid == -1)
    {
        perror("shmget failed");
        exit(1);
    }

    ptr = (int *)shmat(shmid, NULL, 0);

    if (ptr == (void *)-1)
    {
        perror("shmat failed");
        exit(1);
    }

    int ret = fork();

    if (ret == -1)
    {
        perror("fork failed");
        shmdt(ptr);
        exit(1);
    }
    else if (ret == 0)
    {
        for (i = 0; i < 5; i++)
        {
            ptr[i] = (i + 1) * 10;
        }

        printf("Child: data written\n");
        shmdt(ptr);
        exit(0);
    }
    else
    {
        wait(NULL);

        printf("Parent reads: ");
        for (i = 0; i < 5; i++)
        {
            printf("%d ", ptr[i]);
        }

        printf("\n");
        shmdt(ptr);
    }

    return 0;
}

