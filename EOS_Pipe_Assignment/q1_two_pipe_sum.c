#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void)
{
    int p1[2], p2[2];

    if (pipe(p1) == -1 || pipe(p2) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // Child: send two numbers to parent
        close(p1[0]);
        close(p2[1]);

        int a = 10, b = 20;
        write(p1[1], &a, sizeof(a));
        write(p1[1], &b, sizeof(b));
        close(p1[1]);

        // Child: receive the sum
        int sum;
        if (read(p2[0], &sum, sizeof(sum)) == sizeof(sum)) {
            printf("Child: Sum = %d\n", sum);
            fflush(stdout);
        }

        close(p2[0]);
        _exit(0);
    }

    // Parent: receive numbers
    close(p1[1]);
    close(p2[0]);

    int a, b;
    ssize_t r1 = read(p1[0], &a, sizeof(a));
    ssize_t r2 = read(p1[0], &b, sizeof(b));
    close(p1[0]);

    if (r1 != sizeof(a) || r2 != sizeof(b)) {
        fprintf(stderr, "Parent: Failed to read numbers\n");
        close(p2[1]);
        waitpid(pid, NULL, 0);
        return 1;
    }

    int sum = a + b;
    printf("Parent: %d + %d = %d\n", a, b, sum);

    if (write(p2[1], &sum, sizeof(sum)) != sizeof(sum)) {
        perror("write");
        close(p2[1]);
        waitpid(pid, NULL, 0);
        return 1;
    }

    close(p2[1]);

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return 1;
    }

    printf("Parent: Child completed\n");
    return 0;
}
