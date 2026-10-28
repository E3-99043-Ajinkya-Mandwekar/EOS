#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>

int main()
{
	int ret, status;
	printf("Program started\n");
	ret = fork();

	if(ret == 0)
	{
		for(int i = 1; i <= 10; i++)
		{
			printf("child(%d) : i = %d\n", getpid(), i);
			sleep(1);
		}
		return 0;
	}
	else if (ret > 0)
	{
		for(int i = 15; i >= 1; i--)
		{
			if(i == 5)
			{
				waitpid(ret, &status, 0);
				if(WIFEXITED(status))
				{
					printf("Exit status of child = %d\n", WEXITSTATUS(status));
				}
			}

			printf("parent (%d) : i = %d\n", getpid(), i);
			sleep(1);
		}
	}
	else
	{
		perror("fork failed");
		return 1;
	}
	printf("program finished\n");
	return 0;
}

