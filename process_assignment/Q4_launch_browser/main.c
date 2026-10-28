#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    const char *url = (argc >= 2) ? argv[1] : "https://www.google.com";
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        /*
         * execlp searches PATH for the browser command.
         * Try common Linux browser commands in order.
         */
        execlp("xdg-open", "xdg-open", url, (char *)NULL);
        execlp("firefox", "firefox", url, (char *)NULL);
        execlp("google-chrome", "google-chrome", url, (char *)NULL);
        execlp("chromium", "chromium", url, (char *)NULL);

        perror("Could not start a browser (tried xdg-open, firefox, chrome, chromium)");
        _exit(127);
    }

    printf("Parent PID=%ld started browser child PID=%ld for URL: %s\n",
           (long)getpid(), (long)pid, url);

    /* Reap the child; xdg-open may return once it hands off to the browser. */
    if (waitpid(pid, NULL, 0) < 0) {
        perror("waitpid");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
