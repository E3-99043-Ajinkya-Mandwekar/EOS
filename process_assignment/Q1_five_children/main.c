#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define CHILD_COUNT 5

int main(void)
{
    pid_t pid;

    for (int i = 0; i < CHILD_COUNT; i++) {
        pid = fork();

        if (pid < 0) {
            perror("fork");
            /* Wait for children already created before exiting. */
            while (wait(NULL) > 0) {
            }
            return EXIT_FAILURE;
        }

        if (pid == 0) {
            /* Child process: each child counts for 5 seconds. */
            for (int count = 1; count <= 5; count++) {
                printf("Child PID=%ld, count=%d\n", (long)getpid(), count);
                fflush(stdout);
                sleep(1);
            }
            _exit(EXIT_SUCCESS);
        }
    }

    /* Parent: wait for all children and reap them. */
    for (int i = 0; i < CHILD_COUNT; i++) {
        if (wait(NULL) < 0) {
            perror("wait");
            return EXIT_FAILURE;
        }
    }

    printf("Parent: all children completed and cleaned up.\n");
    return EXIT_SUCCESS;
}
