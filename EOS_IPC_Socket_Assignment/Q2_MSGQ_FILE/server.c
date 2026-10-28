#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define KEY 0x4567
#define TEXT 256

struct request {
    long type;
    char fifo[TEXT];
    char filename[TEXT];
};

int main(void)
{
    int qid = msgget(KEY, IPC_CREAT | 0666);
    if (qid == -1) { perror("msgget"); return 1; }

    struct request req;
    printf("File server waiting for request...\n");

    if (msgrcv(qid, &req, sizeof(req) - sizeof(long), 1, 0) == -1) {
        perror("msgrcv");
        return 1;
    }

    /* Basic path restriction: accept only a filename, not a path. */
    if (strchr(req.filename, '/') != NULL) {
        fprintf(stderr, "Please send a filename without directory paths.\n");
        return 1;
    }

    FILE *fp = fopen(req.filename, "rb");
    if (!fp) { perror("fopen requested file"); return 1; }

    int out = open(req.fifo, O_WRONLY);
    if (out == -1) { perror("open client FIFO"); fclose(fp); return 1; }

    char buf[1024];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
        size_t sent = 0;
        while (sent < n) {
            ssize_t w = write(out, buf + sent, n - sent);
            if (w <= 0) {
                perror("write FIFO");
                close(out);
                fclose(fp);
                return 1;
            }
            sent += (size_t)w;
        }
    }

    if (ferror(fp)) perror("fread");
    close(out);
    fclose(fp);
    printf("Sent file: %s\n", req.filename);
    return 0;
}
