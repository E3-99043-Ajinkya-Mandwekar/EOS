#include<stdio.h>
#include<unistd.h>
int main()
{

printf("PROCESS CONTINUE");
int ret = fork();
printf("Return valiue: %d\n", ret);
printf("program finished\n");
return 0;
}



