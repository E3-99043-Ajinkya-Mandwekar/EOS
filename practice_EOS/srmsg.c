#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/msg.h>

#define MSQ_KEY     0x1234

struct msgbuf {
    long mtype;       
    char msg[32];    
};

int main(void)
{
    printf("program started\n");
    //1. create a msg queue
    int msqid = msgget(MSQ_KEY, IPC_CREAT | 0600);
    if(msqid == -1)
    {
        perror("msgget() is failed");
        _exit(msqid);
    }

    int pid = fork();
    if(pid == 0)
    {
        // child process - receveive
        printf("Child [PID = %d] waiting for the message .....\n", getpid());

        struct msgbuf m2;
        int ret = msgrcv(msqid, &m2, sizeof(m2.msg), 11, 0);
        if(ret == -1)
        {
            perror("msgrcv() is failed");
            _exit(ret);
        }
        printf("Child [PID = %d] recieved message : %s\n", getpid(), m2.msg);

        _exit(0);
    }
    else
    {
        // parent process - send
        struct msgbuf m1;
        m1.mtype = 11;
        printf("Parent [PID = %d] : Enter message : ", getpid());
        scanf("%[^\n]s", m1.msg);

        int ret = msgsnd(msqid, &m1, sizeof(m1.msg), 0);
        if(ret == -1)
        {
            perror("msgsnd() is failed");
            _exit(ret);
        }

        printf("Prarent [PID = %d] msg is sent successfully !!!\n", getpid());
    }
    // wait for a child
    int s;
    wait(&s);

    // remove msg queue
    msgctl(msqid, IPC_RMID, NULL);

    printf("program finished\n");

    return 0;
}


























