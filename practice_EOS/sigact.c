#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<signal.h>

void sigint_handler(int signo, siginfo_t *info, void *ucontext)
{
	printf("%d signal is send by process %d\n", info->si_signo, info->si_pid);
	for (int i = 0; i < 5; i++)
	{
		printf("{%d} i = %d\n", getpid(), i);
		sleep(2);
	}
}

int main(){
	struct sigaction act;

	memset(&act, 0, sizeof(struct sigaction));
	act.sa_sigaction = sigint_handler;
	act.sa_flags = SA_SIGINFO;
	sigfillset(&act.sa_mask);
	sigaction(SIGINT, &act, NULL);

	int j = 0;
	while(1)
	{
		printf("[%d] j = %d\n", getpid(), j++);
		sleep(2);
	}
	return 0;
}
