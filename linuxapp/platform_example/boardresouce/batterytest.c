/*************************************************************************
 > File Name: ledtest.c
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "board_resource.h"

int main(int argc, char *argv[])
{

	int fd;
	unsigned int mseconds=1000;
	unsigned int nodeId,oldid = 0;
	BR_RESULT ret = BR_SUCCESS;


	if(argc ==2)
		mseconds=strtoul(argv[1],NULL,10);
	do
	{
		ret = Board_Init(&fd);
		if (ret != BR_SUCCESS)
		{
//				printf("open device fail[%d]\n", ret);
				continue;
		}
		Board_GetBattery(fd, &nodeId);
		Board_DeInit(fd);
		if(oldid == nodeId)
		{
			printf("battery:%d(%xh)\n", nodeId, nodeId);
		}else{
			printf("battery:%d(%xh) changed\n", nodeId, nodeId);
			oldid = nodeId;
		}

		usleep(mseconds*1000);
	} while (1);

	return 0;
}
