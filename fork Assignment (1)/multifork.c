#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int ret, status;

    // Create 5 child processes
    for (int i = 1; i <= 5; i++)
    {
        ret = fork();

        if (ret == 0)
        {
            // Child process
            for (int j = 1; j <= 5; j++)
            {
                printf("Child PID = %d, Count = %d\n",
                       getpid(), j);
                sleep(1);
            }

            return 0;  // Child finishes here
        }
    }

    // Parent process waits for all 5 children
    for (int i = 1; i <= 5; i++)
    {
        wait(&status);
    }

    printf("Parent: All children completed!\n");

    return 0;
}
