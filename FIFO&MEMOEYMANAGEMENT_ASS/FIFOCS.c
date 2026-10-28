#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include <errno.h>
#define fifo_c2s    "client_to_server"
#define fifo_s2c    "server_to_client"

int main()
{
int fd_read, fd_write;
int num1, num2, sum;

if( mkfifo(fifo_c2s, 0666) == -1  && errno != EEXIST)
{
perror("ERROR in c2s");
exit(1);
 
}

if(  mkfifo(fifo_s2c, 0666) == -1 && errno != EEXIST)
{
perror("ERROR in s2c");

exit(1);
}

fd_read = open(fifo_c2s, O_RDONLY);

if(fd_read == -1)
{
perror("open c2s failed");
exit(1);
}

read(fd_read, &num1, sizeof(num1));
read(fd_read, &num2, sizeof(num2));

close(fd_read);

sum = num1 + num2;

fd_write = open(fifo_s2c, O_WRONLY);

if (fd_write == -1)
{
    perror("open s2c failed");
    exit(1);
}
  
write(fd_write, &sum, sizeof(sum));
close(fd_write);

printf("ENTER TWO NUM : ");
scanf("%d\n %d\n", &num1, &num2);

fd_write = open(fifo_c2s, O_WRONLY);

if (fd_write == -1)
{
    perror("open c2s failed");
    exit(1);
}write(fd_write, &sum, sizeof(sum));


return 0;
}


