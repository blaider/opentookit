// #############################################################################
// *****************************************************************************
//                  Copyright (c) 2015, Advantech Automation Corp.
//      THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY
//               INFORMATION WHICH IS THE PROPERTY OF ADVANTECH AUTOMATION CORP.
//
//    ANY DISCLOSURE, USE, OR REPRODUCTION, WITHOUT WRITTEN AUTHORIZATION FROM
//               ADVANTECH AUTOMATION CORP., IS STRICTLY PROHIBITED.
// *****************************************************************************
// #############################################################################
//
// File:   	 ioporttest.c
// Author:  suchao.wang
// Created: Jul 27, 2015
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include<sys/io.h>
#include<unistd.h>

#define EFIR (0x2e)
#define EFDR (EFIR+1)

#define out8(x,y) outb(y,x)
#define in8(x) inb(x)

#define ENTEREXT() \
	out8(EFIR,0x87);\
	out8(EFIR,0x87)
#define EXITEXT()	\
	out8(EFIR,0xaa)
#define SELECTDEV(x)	\
	out8(EFIR,0x07);\
	out8(EFDR,x)
#define ENABLEDEV(x) \
	SELECTDEV(x); \
	out8(EFIR,0x30);\
	out8(EFDR,0x1)
#define DISABLEDEV(x) \
	SELECTDEV(x); \
	out8(EFIR,0x30);\
	out8(EFDR,0x0)
#define READ_REGISTER(x,y) \
	out8(EFIR,x);\
	y=in8(EFDR)
#define WRITE_REGISTER(x,y) \
	out8(EFIR,x);\
	out8(EFDR,y)

void print_usage()
{
	printf("Usage:\n");
	printf("wdttest timeout[ >= 60]		;enable wdt for timeour.for example wdttest 60\n");
	printf("wdttest off 			;disable wdt.for example wdttest off,\n");
	printf("wdttest query			;query  wdt status.0 seconds means disable.\n");
}

int main(int argc,char *argv[])
{
	char dogtime = 60;
	int strokeTime = 10;
	ioperm( EFIR, 2, 1);

	if(argc > 1)
	{
		if(!strcmp(argv[1],"off"))
		{
			ENTEREXT();
			ENABLEDEV(0x8);
			WRITE_REGISTER(0xf6, 0);
			EXITEXT();
			printf("watchdog off\n");
			return 0;
		}else if(!strcmp(argv[1],"query"))
		{
			char left = 0;
			ENTEREXT();
			ENABLEDEV(0x8);
			READ_REGISTER(0xf6, left);
			EXITEXT();
			printf("watchdog restart system left:%3d sceonds\n",left);
			return 0;

		}else
		{
			int time = strtoul(argv[1],NULL,10);
			if(time >= 60)
			{
				dogtime = time;
				strokeTime = time/10;
			}else{
				print_usage();
				return 0;
			}
		}
	}else
	{
		print_usage();
		return 0;
	}

	while (1)
	{
		ENTEREXT();
		ENABLEDEV(0x8);
		WRITE_REGISTER(0xf6, dogtime);
		EXITEXT();
		sleep(strokeTime);
	}
	return 0;
}



