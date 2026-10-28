#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>

int main(){
	printf("program started");
	char buf[64];

	int arr[2];
	int ret = pipe(arr);
	if(ret == -1)
	{
		perror("pipe() i failed");
		_exit(-1);
	}

	int pid = fork();
	if(pid == 0)
	{
		close(arr[1]);
		printf("[%d] waiting for the message....", getpid());
		read(arr[0], buf, sizeof(buf));
		printf("[%d} Received message : %s\n", getpid(), buf);

		close(arr[0]);
		_exit(0);
	}
	else
	{
		close(arr[0]);
		printf("[%d] Enter the message : ", getpid());
		scanf("%[^\n]s", buf);
		write(arr[1], buf, strlen(buf)+1);
		printf("[%d} message is written into pipe\n", getpid());

		close(arr[1]);
	}

	int status;
	waitpid(pid, &status, 0);
	 printf("program finished");
	 return 0;
}

          
         

















