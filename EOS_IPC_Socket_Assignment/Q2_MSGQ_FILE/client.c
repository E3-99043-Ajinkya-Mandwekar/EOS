#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define KEY 0x4567
#define TEXT 256
#define FIFO "/tmp/eos_file_client_fifo"

struct request {
    long type;
    char fifo[TEXT];
    char filename[TEXT];
};

int main(void)
{
    char filename[TEXT];
    char output[TEXT];

    printf("Filename to request from server: ");
    if (scanf("%255s", filename) != 1 || strchr(filename, '/') != NULL) {
        fprintf(stderr, "Enter a filename without a directory path.\n");
        return 1;
    }

    printf("Save received file as: ");
    if (scanf("%255s", output) != 1) return 1;

    if (mkfifo(FIFO, 0600) == -1) {
        /* It may already exist from a previous run. */
        unlink(FIFO);
        if (mkfifo(FIFO, 0600) == -1) {
            perror("mkfifo");
            return 1;
        }
    }

    int qid = msgget(KEY, 0666);
    if (qid == -1) { perror("msgget: start server first"); unlink(FIFO); return 1; }

    struct request req = { .type = 1 };
    snprintf(req.fifo, sizeof(req.fifo), "%s", FIFO);
    snprintf(req.filename, sizeof(req.filename), "%s", filename);

    if (msgsnd(qid, &req, sizeof(req) - sizeof(long), 0) == -1) {
        perror("msgsnd");
        unlink(FIFO);
        return 1;
    }

    /* Opening read-only waits until server opens the FIFO for writing. */
    int in = open(FIFO, O_RDONLY);
    if (in == -1) { perror("open FIFO"); unlink(FIFO); return 1; }

    FILE *fp = fopen(output, "wb");
    if (!fp) { perror("fopen output"); close(in); unlink(FIFO); return 1; }

    char buf[1024];
    ssize_t n;
    while ((n = read(in, buf, sizeof(buf))) > 0) {
        if (fwrite(buf, 1, (size_t)n, fp) != (size_t)n) {
            perror("fwrite");
            fclose(fp);
            close(in);
            unlink(FIFO);
            return 1;
        }
    }

    if (n == -1) perror("read");
    fclose(fp);
    close(in);
    unlink(FIFO);
    printf("File saved as: %s\n", output);
    return n == -1 ? 1 : 0;
}
