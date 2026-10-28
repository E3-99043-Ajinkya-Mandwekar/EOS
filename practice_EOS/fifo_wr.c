#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>

int main(void)
{
	int fd = open("/home/sunbeam/fifo", O_WRONLY);

	char buf[64];

	printf("Enter message : ");
	scanf[%[^\n]s", buf);

	write(fd, buf, strlen(buf)+1);
	printf("message is wriien successfully\n");

	close(fd);

	return 0;
}

