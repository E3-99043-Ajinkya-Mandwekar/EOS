#include<stdio.h>
#include<unistd.h>
#include<signal.h>

void signal_handler(int sig)
{
	if(sig == SIGINT)
	printf("SIGINT signal is caught : %d\n", sig);
	else if(sig == SIGTERM)
		printf("SIGTERM signal is caught : %d\n", sig);
	else if(sig == SIGALRM)
		printf("SIGALRM signal is caught : %d\n", sig);
	else
		printf("Siganl is caught : %d\n", sig);
}

void sigsegv_handler(int sig)
{
	printf("SIGSEGV signal is caught : %d\n", sig);
	_exit(1);
}

int main()
{
	signal(SIGINT, signal_handler);
	 signal(SIGTERM, signal_handler);
 signal(SIGALRM, signal_handler);
signal(SIGSEGV, sigsegv_handler);
 int i = 0;
 while(1)
 {
	 printf("i = %d\n", i++);
	 sleep(2);
 }
 return 0;
}

