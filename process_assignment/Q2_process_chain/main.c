#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

static void run_for_five_seconds(char name)
{
    time_t start = time(NULL);
    int count = 1;

    while (time(NULL) - start < 5) {
        printf("Process %c: PID=%ld, PPID=%ld, count=%d\n",
               name, (long)getpid(), (long)getppid(), count++);
        fflush(stdout);
        sleep(1);
    }
}

int main(void)
{
    pid_t b, c, d;

    b = fork();
    if (b < 0) {
        perror("fork B");
        return EXIT_FAILURE;
    }

    if (b == 0) {
        /* Process B creates C. */
        c = fork();
        if (c < 0) {
            perror("fork C");
            _exit(EXIT_FAILURE);
        }

        if (c == 0) {
            /* Process C creates D. */
            d = fork();
            if (d < 0) {
                perror("fork D");
                _exit(EXIT_FAILURE);
            }

            if (d == 0) {
                run_for_five_seconds('D');
                _exit(EXIT_SUCCESS);
            }

            run_for_five_seconds('C');
            if (waitpid(d, NULL, 0) < 0)
                perror("waitpid D");
            _exit(EXIT_SUCCESS);
        }

        run_for_five_seconds('B');
        if (waitpid(c, NULL, 0) < 0)
            perror("waitpid C");
        _exit(EXIT_SUCCESS);
    }

    /* Original parent A also runs concurrently. */
    run_for_five_seconds('A');
    if (waitpid(b, NULL, 0) < 0) {
        perror("waitpid B");
        return EXIT_FAILURE;
    }

    printf("Parent A: process chain completed and cleaned up.\n");
    return EXIT_SUCCESS;
}
