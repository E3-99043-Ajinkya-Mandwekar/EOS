#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define C2S "/tmp/eos_sum_c2s"
#define S2C "/tmp/eos_sum_s2c"

struct numbers {
    int a, b;
};

int main(void)
{
    mkfifo(C2S, 0666);
    mkfifo(S2C, 0666);

    printf("FIFO sum server waiting for client...\n");

    int in = open(C2S, O_RDONLY);
    if (in == -1) { perror("open client FIFO"); return 1; }

    struct numbers n;
    if (read(in, &n, sizeof(n)) != sizeof(n)) {
        perror("read");
        close(in);
        return 1;
    }
    close(in);

    int sum = n.a + n.b;
    int out = open(S2C, O_WRONLY);
    if (out == -1) { perror("open server FIFO"); return 1; }

    if (write(out, &sum, sizeof(sum)) != sizeof(sum))
        perror("write");

    close(out);
    printf("Server: %d + %d = %d\n", n.a, n.b, sum);
    return 0;
}
