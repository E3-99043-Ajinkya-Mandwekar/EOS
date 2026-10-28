#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void)
{
    int arr[2];

    if (pipe(arr) == -1) {
        perror("pipe");
        return 1;
    }

    // First child: who
    pid_t pid1 = fork();

    if (pid1 == -1) {
        perror("fork");
        return 1;
    }

    if (pid1 == 0) {
        close(arr[0]);

        // Redirect stdout to pipe
        if (dup2(arr[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            _exit(1);
        }

        close(arr[1]);
        execlp("who", "who", (char *)NULL);

        perror("execlp who");
        _exit(127);
    }

    // Second child: wc
    pid_t pid2 = fork();

    if (pid2 == -1) {
        perror("fork");
        close(arr[0]);
        close(arr[1]);
        waitpid(pid1, NULL, 0);
        return 1;
    }

    if (pid2 == 0) {
        close(arr[1]);

        // Redirect stdin from pipe
        if (dup2(arr[0], STDIN_FILENO) == -1) {
            perror("dup2");
            _exit(1);
        }

        close(arr[0]);
        execlp("wc", "wc", (char *)NULL);

        perror("execlp wc");
        _exit(127);
    }

    // Parent does not use the pipe
    close(arr[0]);
    close(arr[1]);

    int s1, s2;
    waitpid(pid1, &s1, 0);
    waitpid(pid2, &s2, 0);

    return 0;
}
