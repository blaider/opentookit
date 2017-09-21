/*************************************************************************
 > File Name: diotest.c
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include "board_resource.h"

int main(int argc, char *argv[])
{
	int fd;
	BR_RESULT ret = BR_SUCCESS;
	ret = DIO_Init(&fd);
	if (ret != BR_SUCCESS)
		printf("open device fail[%d]\n", ret);

	int DI = Get_DICount();
	int DO = Get_DOCount();
	printf("DI:%d,DO:%d\n",DI,DO);
	unsigned int value;

	int i = 0;
	while(1)
	{
		if(DI)
		{
			Di_Read(fd,0,DI,&value);
			printf("DI:%08x\n",value);
		}
		if(DO)
		{
			for(i=0;i< DO;i++)
			{
				Do_Read(fd,i,1,&value);
				value = ~value;
				Do_Write(fd,i,1,value);
				Do_Read(fd,i,1,&value);
				printf("DO[%d]:%04x\n",i,value);
				sleep(1);
			}
		}

		sleep(3);
	}

	return 0;
}
