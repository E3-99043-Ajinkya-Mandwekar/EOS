
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define RAW_FILE "RawData.txt"
#define ARCHIVE_FILE "ArchivedData.txt"
#define LINK_FILE "ArchiveLink.txt"

int write_all(int fd, const char *buf, size_t len)
{
    size_t done = 0;

    while (done < len) {
        ssize_t n = write(fd, buf + done, len - done);

        if (n < 0) {
            if (errno == EINTR)
                continue;
            return -1;
        }

        if (n == 0)
            return -1;

        done += (size_t)n;
    }

    return 0;
}

/* W1: Generate data and write it to RawData.txt */
int worker_w1(void)
{
    const char data[] =
        "Device status: ONLINE\n"
        "Temperature: 28.5 C\n"
        "Voltage: 3.30 V\n";

    int fd = open(RAW_FILE,
                  O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd < 0) {
        perror("open RawData.txt");
        return 1;
    }

    if (write_all(fd, data, sizeof(data) - 1) < 0) {
        perror("write RawData.txt");
        close(fd);
        return 1;
    }

    close(fd);
    printf("W1: Raw data generated.\n");
    return 0;
}

/* W2: Read RawData.txt and send data through the pipe */
int worker_w2(int pipefd)
{
    char buf[256];
    int fd = open(RAW_FILE, O_RDONLY);

    if (fd < 0) {
        perror("W2 open");
        close(pipefd);
        return 1;
    }

    for (;;) {
        ssize_t n = read(fd, buf, sizeof(buf));

        if (n == 0)
            break;

        if (n < 0) {
            if (errno == EINTR)
                continue;

            perror("W2 read");
            close(fd);
            close(pipefd);
            return 1;
        }

        if (write_all(pipefd, buf, (size_t)n) < 0) {
            perror("W2 pipe write");
            close(fd);
            close(pipefd);
            return 1;
        }
    }

    close(fd);
    close(pipefd);

    printf("W2: Data sent to controller.\n");
    return 0;
}

/* W3's new program image: create a hard link */
int link_worker(const char *source, const char *destination)
{
    if (link(source, destination) < 0) {
        perror("link");
        return 1;
    }

    printf("W3: Hard link created: %s\n", destination);
    return 0;
}

int wait_child(pid_t pid)
{
    int status;

    while (waitpid(pid, &status, 0) < 0) {
        if (errno == EINTR)
            continue;

        perror("waitpid");
        return -1;
    }

    return WIFEXITED(status) && WEXITSTATUS(status) == 0
           ? 0 : -1;
}

int main(int argc, char *argv[])
{
    /* This mode is entered when W3 calls exec(). */
    if (argc == 4 && strcmp(argv[1], "--link") == 0)
        return link_worker(argv[2], argv[3]);

    int pipefd[2];
    char buf[256];
    int archivefd;
    pid_t w1, w2, w3;

    if (pipe(pipefd) < 0) {
        perror("pipe");
        return 1;
    }

    /* Create W1 */
    w1 = fork();

    if (w1 < 0) {
        perror("fork W1");
        return 1;
    }

    if (w1 == 0) {
        close(pipefd[0]);
        close(pipefd[1]);
        _exit(worker_w1());
    }

    /* Ensure RawData.txt is ready before W2 reads it. */
    close(pipefd[0]);
    close(pipefd[1]);

    if (wait_child(w1) < 0)
        return 1;

    /* Create a fresh pipe for W2 -> P. */
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return 1;
    }

    w2 = fork();

    if (w2 < 0) {
        perror("fork W2");
        return 1;
    }

    if (w2 == 0) {
        close(pipefd[0]);
        _exit(worker_w2(pipefd[1]));
    }

    /* Controller P receives data and archives it. */
    close(pipefd[1]);

    archivefd = open(ARCHIVE_FILE,
                     O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (archivefd < 0) {
        perror("open archive");
        close(pipefd[0]);
        wait_child(w2);
        return 1;
    }

    for (;;) {
        ssize_t n = read(pipefd[0], buf, sizeof(buf));

        if (n == 0)
            break;

        if (n < 0) {
            if (errno == EINTR)
                continue;

            perror("P read pipe");
            close(pipefd[0]);
            close(archivefd);
            wait_child(w2);
            return 1;
        }

        if (write_all(archivefd, buf, (size_t)n) < 0) {
            perror("P write archive");
            close(pipefd[0]);
            close(archivefd);
            wait_child(w2);
            return 1;
        }
    }

    close(pipefd[0]);
    close(archivefd);

    if (wait_child(w2) < 0)
        return 1;

    /* Create W3 and replace its process image using exec(). */
    w3 = fork();

    if (w3 < 0) {
        perror("fork W3");
        return 1;
    }

    if (w3 == 0) {
        execlp(argv[0], argv[0], "--link",
               ARCHIVE_FILE, LINK_FILE, (char *)NULL);

        perror("exec");
        _exit(127);
    }

    if (wait_child(w3) < 0)
        return 1;

    printf("P: All workers completed successfully.\n");
    return 0;
}

