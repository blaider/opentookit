/*************************************************************************
	> File Name: modem_monitor_main.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Mon 05 Nov 2018 01:27:59 PM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include "Log.h"

#include "modem_scan.h"


void printf_usage()
{
	printf("Usage:\n ./modemmonitor -d -p /dev/ttyUSB0\n");
	printf(" -d 	           ;run as daemon\n");
	printf(" -p /dev/ttyUSB1    ;device port   \n");
}

int main(int argc, char *argv[])
{
	char pname[256];
	int run_daemon = 0;
	int ch;

	memset(pname,0,sizeof(pname));
	while ((ch = getopt(argc, argv, "dp:")) != -1)
	{
//		printf("optind: %d\n", optind);
		switch (ch)
		{
		case 'd':
//			printf("HAVE option: -d\n");
			run_daemon = 1;
			break;
		case 'p':
//			printf("HAVE option: -p %s\n", optarg);
			strcpy(pname,optarg);
			break;
		case '?':
			printf("Unknown option: %c\n", (char) optopt);
			printf_usage();
			return -1;
		}
	}

	if(!strlen(pname) || access(pname,F_OK))
	{
		LOGEX("[%s] not exist\n",pname);
		printf_usage();
		return -1;
	}

	if (run_daemon)
		daemon(0, 0);
//	if(isatty)
	start_modem_scan(pname);

	return 0;
}
