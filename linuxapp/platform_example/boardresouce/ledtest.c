/*************************************************************************
 > File Name: ledtest.c
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "board_resource.h"

int main(int argc, char *argv[])
{
	int fd =0;
	BR_RESULT ret = BR_SUCCESS;
	ret = LED_Init(LED_TYPE_RUN, &fd);
	if (ret != BR_SUCCESS)
	{
		printf("open LED_TYPE_RUN[P1] fail[%d]\n", ret);
		return -1;
	}
	LED_On(fd);
	sleep(1);
	LED_Off(fd);
	LED_DeInit(fd);

	return 0;
}
