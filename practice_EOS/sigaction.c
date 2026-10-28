#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void sigint_handler(int sig)
{
	printf("SIGINT signal is received : %d\n", sig);
}

int main(void){
	struct sigaction act, oldact;
	act.sa_handler = sigint_handler;
	sigaction(SIGINT, &act, &oldact);

	int i = 0;
	while(1){
		printf("i = %d\n", i++);
		sleep(1);
	}
	return 0;
}

