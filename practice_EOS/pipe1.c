#include<stdio.h>
#include<unistd.h>

int main(){
	int arr[2];
	int ret = pipe(arr);
	if(ret == -1)
	{
		perror("pipe()is failed");
		_exit(ret);
	}

	char buf1[64] = "GOOD MORNING";
	char buf2[64];

	printf("[%d] writing message into pipe...\n", getpid());
	write(arr[1], buf1, 64);
	printf("[%d]written message = %s\n", getpid(), buf1);
	printf("[%d] Reading Message from pipe...\n",getpid(), buf1);
	read(arr[0], buf2, 64);
	printf("[%d] Read message = %s\n", getpid(),buf2);

	close(arr[0]);
	close(arr[1]);
	return 0;
}
