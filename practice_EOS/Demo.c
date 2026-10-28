#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define RAW_FILE     "RawData.txt"
#define ARCHIVE_FILE "ArchivedData.txt"
#define LINK_FILE    "ArchiveLink.txt"

int write_all(int fd, const void *buffer, size_t length)
{
    const char *p = buffer;
    size_t total = 0;

    while (total < length) {
        ssize_t n = write(fd, p + total, length - total);

        if (n == -1) {
            if (errno == EINTR)
                continue;
            return -1;
        }

        if (n == 0)
            return -1;

        total += (size_t)n;
    }

    return 0;
}

/* W1: Generate data and write to RawData.txt */
void worker1(int pipefd[2])
{
    close(pipefd[0]);
    close(pipefd[1]);

    const char data[] =
        "Device Status: NORMAL\n"
        "Speed: 45 km/h\n"
        "Temperature: 36 C\n";

    int fd = open(RAW_FILE,
                  O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        perror("W1: open");
        _exit(1);
    }

    if (write_all(fd, data, strlen(data)) == -1) {
        perror("W1: write");
        close(fd);
        _exit(1);
    }

    if (close(fd) == -1) {
        perror("W1: close");
        _exit(1);
    }

    printf("W1: RawData.txt created successfully\n");
    fflush(stdout);
    _exit(0);
}

/* W2: Read RawData.txt and send data through the pipe */
void worker2(int pipefd[2])
{
    close(pipefd[0]);

    int fd = open(RAW_FILE, O_RDONLY);

    if (fd == -1) {
        perror("W2: open");
        close(pipefd[1]);
        _exit(1);
    }

    char buffer[128];

    while (1) {
        ssize_t n = read(fd, buffer, sizeof(buffer));

        if (n == -1) {
            if (errno == EINTR)
                continue;

            perror("W2: read");
            close(fd);
            close(pipefd[1]);
            _exit(1);
        }

        if (n == 0)
            break;

        if (write_all(pipefd[1], buffer, (size_t)n) == -1) {
            perror("W2: pipe write");
            close(fd);
            close(pipefd[1]);
            _exit(1);
        }
    }

    if (close(fd) == -1) {
        perror("W2: close file");
        close(pipefd[1]);
        _exit(1);
    }

    if (close(pipefd[1]) == -1) {
        perror("W2: close pipe");
        _exit(1);
    }

    printf("W2: Data transferred through pipe\n");
    fflush(stdout);
    _exit(0);
}

/* W3's new program image: create a hard link */
int create_hard_link(void)
{
    /*
     * Remove the previous link so the program can
     * run repeatedly. Do not remove the archive.
     */
    if (unlink(LINK_FILE) == -1 && errno != ENOENT) {
        perror("W3: unlink old link");
        return 1;
    }

    if (link(ARCHIVE_FILE, LINK_FILE) == -1) {
        perror("W3: link");
        return 1;
    }

    printf("W3: Hard link ArchiveLink.txt created\n");
    return 0;
}

int main(int argc, char *argv[])
{
    /* This mode runs after exec() replaces W3's image */
    if (argc == 2 && strcmp(argv[1], "--link") == 0)
        return create_hard_link();

    int pipefd[2];

    /* Controller P creates the pipe */
    if (pipe(pipefd) == -1) {
        perror("P: pipe");
        return 1;
    }

    /* Create W1 */
    pid_t w1 = fork();

    if (w1 == -1) {
        perror("P: fork W1");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }

    if (w1 == 0)
        worker1(pipefd);

    int status;

    /* W1 must finish before W2 reads the file */
    if (waitpid(w1, &status, 0) == -1) {
        perror("P: waitpid W1");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "P: W1 failed\n");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }

    /* Create W2 */
    pid_t w2 = fork();

    if (w2 == -1) {
        perror("P: fork W2");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }

    if (w2 == 0)
        worker2(pipefd);

    /* P only reads from the pipe */
    if (close(pipefd[1]) == -1) {
        perror("P: close pipe write end");
        close(pipefd[0]);
        waitpid(w2, NULL, 0);
        return 1;
    }

    int archive_fd = open(ARCHIVE_FILE,
                          O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (archive_fd == -1) {
        perror("P: open archive");
        close(pipefd[0]);
        waitpid(w2, NULL, 0);
        return 1;
    }

    char buffer[128];
    int transfer_error = 0;

    while (1) {
        ssize_t n = read(pipefd[0], buffer, sizeof(buffer));

        if (n == -1) {
            if (errno == EINTR)
                continue;

            perror("P: pipe read");
            transfer_error = 1;
            break;
        }

        if (n == 0)
            break;

        if (write_all(archive_fd, buffer, (size_t)n) == -1) {
            perror("P: archive write");
            transfer_error = 1;
            break;
        }
    }

    if (close(pipefd[0]) == -1) {
        perror("P: close pipe read end");
        transfer_error = 1;
    }

    if (close(archive_fd) == -1) {
        perror("P: close archive");
        transfer_error = 1;
    }

    if (waitpid(w2, &status, 0) == -1) {
        perror("P: waitpid W2");
        return 1;
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        transfer_error = 1;

    if (transfer_error) {
        fprintf(stderr, "P: Data transfer failed\n");
        return 1;
    }

    printf("P: Data received and saved in ArchivedData.txt\n");

    /* Create W3 */
    pid_t w3 = fork();

    if (w3 == -1) {
        perror("P: fork W3");
        return 1;
    }

    if (w3 == 0) {
        execl("/proc/self/exe", "Demo.out",
              "--link", (char *)NULL);

        perror("W3: execl");
        _exit(1);
    }

    if (waitpid(w3, &status, 0) == -1) {
        perror("P: waitpid W3");
        return 1;
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "P: W3 failed\n");
        return 1;
    }

    printf("P: All workers completed successfully\n");
    return 0;
}
