#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 5000

struct numbers {
    int a, b;
};

int main(void)
{
    struct numbers n;
    printf("Enter two integers: ");
    if (scanf("%d %d", &n.a, &n.b) != 2) return 1;

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) { perror("socket"); return 1; }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) != 1) {
        perror("inet_pton"); close(s); return 1;
    }

    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect: start server first");
        close(s);
        return 1;
    }

    if (write(s, &n, sizeof(n)) != sizeof(n)) {
        perror("write"); close(s); return 1;
    }

    int sum;
    if (read(s, &sum, sizeof(sum)) == sizeof(sum))
        printf("Client: sum = %d\n", sum);
    else
        fprintf(stderr, "Failed to receive sum\n");

    close(s);
    return 0;
}
