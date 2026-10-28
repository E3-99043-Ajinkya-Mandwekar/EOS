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

    struct numbers n;
    printf("Enter two integers: ");
    if (scanf("%d %d", &n.a, &n.b) != 2) return 1;

    int out = open(C2S, O_WRONLY);
    if (out == -1) { perror("open"); return 1; }
    if (write(out, &n, sizeof(n)) != sizeof(n)) perror("write");
    close(out);

    int in = open(S2C, O_RDONLY);
    if (in == -1) { perror("open"); return 1; }

    int sum;
    if (read(in, &sum, sizeof(sum)) == sizeof(sum))
        printf("Client: sum = %d\n", sum);
    else
        perror("read");

    close(in);
    return 0;
}
