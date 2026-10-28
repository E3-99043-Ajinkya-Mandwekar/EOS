#include<stdio.h>
#include<unistd.h>
#include<signal.h>

void sigint_handler(int sig)
{
	printf("SIGINT signal is caught %d\n", sig);
}

int main(void)
{
	signal(SIGINT, sigint_handler);

	int i;
	while(1)
	{
		printf("i = %d\n", i++);
		sleep(2);
	}
	return 0;
}
