#include<stdio.h>
#include<unistd.h>
int main()
{
	int ret;
	printf("Program started\n");
	ret = fork();
	if(ret == 0){
		for(int i = 1; i <= 10; i++){
			printf("child(%d): %d\n", getpid(), i);
			sleep(1);
		}
	}
        else
	{
		            for(int i = 15; i >= 1; i--){
                        printf("parent(%d): %d\n", getpid(), i);
			sleep(1);
			    }
	}
	printf("Program finished\n");
	return 0;
	}


