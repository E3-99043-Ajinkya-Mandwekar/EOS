#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

/*
 * Demonstration only. There is no single fixed maximum for all Linux systems.
 * This program creates children until fork() fails, then reaps those created.
 * Run only on a test machine; it can temporarily exhaust process resources.
 */

int main(void)
{
    size_t created = 0;
    pid_t pid;

    /* Avoid buffering duplicate output if fork() succeeds. */
    setvbuf(stdout, NULL, _IONBF, 0);

    for (;;) {
        pid = fork();

        if (pid < 0) {
            perror("fork stopped");
            break;
        }

        if (pid == 0) {
            /* Children exit immediately; only the original parent keeps forking. */
            _exit(EXIT_SUCCESS);
        }

        created++;
    }

    printf("Parent PID=%ld created %zu child processes before fork() failed.\n",
           (long)getpid(), created);

    /* Clean up every child created. */
    while (wait(NULL) > 0) {
    }

    if (errno != ECHILD && errno != 0) {
        /* wait() normally ends with ECHILD once all children are reaped. */
    }

    printf("Parent: all created children have been reaped.\n");
    return EXIT_SUCCESS;
}
