#include<stdio.h>
#include<unistd.h>

int main(){

printf("program started\n");
int ret = fork();
printf("Return value = %d\n", ret);
printf("PID = %d\n, PPID = %d\n", getpid(), getppid());
printf("program finished\n");
sleep(2);
return 0;
}
