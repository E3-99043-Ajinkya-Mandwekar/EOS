#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/msg.h>

#define MSG_KEY  0x1234

struct msgbuf {
	long mtype;
	char msg[32];
};

int main(void)
{
	printf("Program Started\n");
	int msqid = msgget(MSQ_KEY, IPC_CRET | 0600);
	if(msqid == -1)
	{
		perror("msggwt() is failed);
		_exit(msqid);
	}
