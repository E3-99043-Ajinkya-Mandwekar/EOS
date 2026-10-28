#include<stdio.h>
#include<sys/stat.h>

int main(int argc, char *argv[])
{
	if(argc != 2)
	{ 
		printf("error");
		return 1;
	}

	const char *filename = argv[1];

	int ret = chmod(filename, 0644);

	if(ret == -1)
	{
		perror("chmod");
		return 1;
	}

	printf("Permission of %s changed to rw-r--r--\n", filename);

return 0;
}
