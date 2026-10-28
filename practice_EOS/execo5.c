#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>

int main()
{
	int status;

	int pid = fork();
	if(pid == 0)
	{
		char *argv[] = {"env.out", "one", "two", NULL};
		char *envp[] = {"MYHOME=/home/sunbeam", "MYUSER=sunbeam", "MYLOGNAME=sunbeam","MYVAR=value", NULL};
		int ret = execve("./env.out", argv, envp);
		if(ret == -1)
		{
			printf("execve() is failed to load a program\n");
			_exit(ret);
		}
	}
	else
	{
		waitpid(pid, &status,0);
		printf("EXIT value of child process : %d\n", WEXITSTATUS(status));
	}

	return 0;
}

