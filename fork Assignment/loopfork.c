#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int ret, status;

    printf("Process A started, PID = %d\n", getpid());
    fflush(stdout);

    ret = fork();             // A creates B

    if (ret == 0)
    {
        // Process B
        printf("Process B started, PID = %d\n", getpid());
        fflush(stdout);

        ret = fork();         // B creates C

        if (ret == 0)
        {
            // Process C
            printf("Process C started, PID = %d\n", getpid());
            fflush(stdout);

            ret = fork();     // C creates D

            if (ret == 0)
            {
                // Process D
                for (int i = 1; i <= 5; i++)
                {
                    printf("D: PID = %d, Count = %d\n",
                           getpid(), i);
                    sleep(1);
                }

                return 0;     // D terminates
            }
            else if (ret > 0)
            {
                // C waits for D
                for (int i = 1; i <= 5; i++)
                {
                    printf("C: PID = %d, Count = %d\n",
                           getpid(), i);
                    sleep(1);
                }

                waitpid(ret, &status, 0);
                return 0;     // C terminates
            }
            else
            {
                perror("fork failed in C");
                return 1;
            }
        }
        else if (ret > 0)
        {
            // B waits for C
            for (int i = 1; i <= 5; i++)
            {
                printf("B: PID = %d, Count = %d\n",
                       getpid(), i);
                sleep(1);
            }

            waitpid(ret, &status, 0);
            return 0;         // B terminates
        }
        else
        {
            perror("fork failed in B");
            return 1;
        }
    }
    else if (ret > 0)
    {
        // A waits for B
        for (int i = 1; i <= 5; i++)
        {
            printf("A: PID = %d, Count = %d\n",
                   getpid(), i);
            sleep(1);
        }

        waitpid(ret, &status, 0);
        printf("A: All processes completed.\n");
    }
    else
    {
        perror("fork failed in A");
        return 1;
    }

    return 0;
}
