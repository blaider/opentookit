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
// File:   	 gpioset.c
// Author:  suchao.wang
// Created: May 27, 2015
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



int initGPIO(const char *name,off_t offset,off_t maxoffset)
{
	int mapfd =-1;
	void * viraddr = NULL;
	if(mapfd < 0)
	{
		mapfd = open("/dev/mem", O_RDWR | O_SYNC); //鎵撳紑mem鏂囦欢锛岃闂洿鎺ュ湴鍧€
		if (mapfd == -1)
		{
			return -1;
		}
	}
	viraddr = mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, mapfd, offset);

	int i=0;

	printf("registers:%s\n",name);
	for(i=0;i<=maxoffset;i+=4)
	{
		volatile unsigned int *gpio_dout = (volatile unsigned int *)((volatile unsigned char *)viraddr + i);
		unsigned int reg = *((volatile unsigned int *)gpio_dout);
		if(reg)
			printf("reg:0x%02x=%08x\n",i,reg);
	}

	close(mapfd);

	return 0;
}

#define CPSW_SS 0x4A100000
#define CPSW_PORT 0x4A100100
#define CPSW_CPDMA 0x4A100800
#define CPSW_STATS 0x4A100900
int main()
{
	initGPIO("CPSW_SS",CPSW_SS,0x30);
	initGPIO("CPSW_PORT",CPSW_SS,0x24c);
//	initGPIO("CPSW_CPDMA",CPSW_CPDMA,0xfc);
	initGPIO("CPSW_STATS",CPSW_STATS,0x8c);
//	restartUSB();
	return 0;
}
