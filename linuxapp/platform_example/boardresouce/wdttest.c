/*************************************************************************
 > File Name: wdttest.c
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <string.h>
#include "board_resource.h"

#define TIMEOUT 10

int main(int argc, char *argv[])
{
	BR_HANDLE wdt_fd = 0;
	int timeout = TIMEOUT;
	BR_RESULT ret = BR_SUCCESS;
	//init handle
	printf("init handle\n");
	ret = WDT_Init(&wdt_fd);
	if (ret != BR_SUCCESS)
	{
		printf("open device fail[%d]\n", ret);
		return 0;
	}
	//enable watch dog
	ret = WDT_Enable(wdt_fd,timeout);
	if (ret != BR_SUCCESS)
	{
		printf("enable wdt fail[%d]\n", ret);
		return 0;
	}

	printf("press Ctrl+C in %d second,the wdt will reboot system\n",timeout);
	while(timeout--){
		//strobe dog
		ret = WDT_Strobe(wdt_fd);
		if (ret == BR_SUCCESS)
		{
			printf("strobe wdt success[%d]\n",timeout);
		}
		sleep(1);
	}

	sleep(5);

	//disable dog
	printf("disable wdt\n");
	ret = WDT_Disable(wdt_fd);
	if (ret != BR_SUCCESS)
	{
		printf("disable wdt fail[%d]\n", ret);
		return 0;
	}
	//uninit handle
	WDT_DeInit(wdt_fd);
	printf("test over\n");
	return 0;
}
