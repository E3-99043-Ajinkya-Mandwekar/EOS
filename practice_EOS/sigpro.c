#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<signal.h>

void signal_handler(int signo)
{
    printf("%d signal is caught [PID = %d]\n", signo, getpid());
}
int main(){
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGTERM);
	sigaddset(&set, SIGALRM);

	int ret = sigprocmask(SIG_SETMASK, &set, NULL);
	if(ret == -1)
	{
		printf("failed to change the process signal mask");
		_exit(ret);
	}

	struct sigaction sa;
	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = signal_handler;
	ret = sigaction(SIGTERM, &sa, NULL);
	if(ret == -1)
	{
		perror("failed to change signal action");
		_exit(ret);
	}
	
	ret = sigaction(SIGALRM, &sa, NULL);
	if(ret == -1)
	{
		perror("failed to change siganl acion");
		_exit(ret);
	}

	int i = 0;
	while(1)
	{
		printf("[PID = %d] i = %d\n", getpid(), i++);
		sleep(2);
	}
	return 0;
}
