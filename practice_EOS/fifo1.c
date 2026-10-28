#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>

int main(void)
{
	int fd = open("/home/sunbeam/fifo", O_WRONLY);

	char buf[64];

	printf("Enter the message : ");
	scanf("%d[^\n]s", buf);

	write(fd, buf, strlen(buf) + 1);
	printf("Message is written successfully\n");

	close(fd);

	return 0;
}

