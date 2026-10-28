#include<stdio.h>
#include<unistd.h>

int main(void)
{
	int ret;
	printf("program started !\n");
	ret = fork();
	if(ret == 0)
	{
		for(int i = 1; i <= 15; i++)
		{
			printf("child (%d); i = %d\n", getpid(), i);
		}
	}
	else
	{
		for(int i = 15; i  >= 2; i--)
		{
			printf("parent(%d) : i = %d\n", getpid(), i);
		}

	}
	printf("program ended\n");
	return 0;
}
