#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

void show()
{
	printf("show\n");
	return ;
}
int main(int argc, char * argv[])
{
	int result;
	if (argc == 2)
	{
		result = access(argv[1], F_OK);
		printf("F_OK:result:%d\n", result);
		result = access(argv[1], R_OK);
		printf("R_OK:result:%d\n", result);
		result = access(argv[1], W_OK);
		printf("W_OK:result:%d\n", result);
		result = access(argv[1], X_OK);
		printf("X_OK:result:%d\n", result);
	}
	printf("%s,%s\n", __func__, strerror(errno));
	show();
	return 0;
}

