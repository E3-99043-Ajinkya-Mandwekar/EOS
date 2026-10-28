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
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1 ||
        listen(s, 1) == -1) {
        perror("bind/listen");
        close(s);
        return 1;
    }

    printf("TCP server listening on port %d...\n", PORT);
    int c = accept(s, NULL, NULL);
    if (c == -1) { perror("accept"); close(s); return 1; }

    struct numbers n;
    if (read(c, &n, sizeof(n)) != sizeof(n)) {
        fprintf(stderr, "Failed to receive two integers\n");
        close(c); close(s); return 1;
    }

    int sum = n.a + n.b;
    if (write(c, &sum, sizeof(sum)) != sizeof(sum))
        perror("write");

    printf("Server: %d + %d = %d\n", n.a, n.b, sum);
    close(c);
    close(s);
    return 0;
}
