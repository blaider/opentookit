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

#define GPIO3_BASE 0x481AE000

void gpio_set(void * addr,unsigned int offset)
{
	volatile unsigned int *gpio_dout = (volatile unsigned int *)((volatile unsigned char *)addr + 0x13c);
    unsigned int reg = *((volatile unsigned int *)gpio_dout);
    reg |= (1 << offset);
	*((volatile unsigned int *)gpio_dout) = reg;

	 printf("%s:%x\n",__func__,*((volatile unsigned int *)gpio_dout));
	 return;
}
void gpio_clear(void * addr,unsigned int offset)
{
	volatile unsigned int *gpio_dout = (volatile unsigned int *)((volatile unsigned char *)addr + 0x13c);
    unsigned int reg = *((volatile unsigned int *)gpio_dout);
    reg &= ~(1 << offset);
	*((volatile unsigned int *)gpio_dout) = reg;

	 printf("%s:%x\n",__func__,*((volatile unsigned int *)gpio_dout));
	 return;
}
int mapfd =-1;
void * viraddr = NULL;
int uninitGPIO()
{
	close(mapfd);
	mapfd = -1;
	return 0;
}
int initGPIO()
{
	if(mapfd < 0)
	{
		mapfd = open("/dev/mem", O_RDWR | O_SYNC); //鎵撳紑mem鏂囦欢锛岃闂洿鎺ュ湴鍧€
		if (mapfd == -1)
		{
			return -1;
		}
	}
	viraddr = mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, mapfd, GPIO3_BASE);

	volatile unsigned int *gpio_oe = (volatile unsigned int *)((volatile unsigned char *)viraddr + 0x134);
    unsigned int reg = *((volatile unsigned int *)gpio_oe);
    reg &= ~(1 << 15);
	*((volatile unsigned int *)gpio_oe) = reg;

	return 0;
}
int restartUSB(int on)
{
	int i=2;
	gpio_clear(viraddr,15);
	while(i--)
	{
		system("lsusb");
		sleep(1);
	}
	if(on)
	gpio_set(viraddr,15);
	return 0;

}
int main(int argc,char *argv[])
{
	int on = 0;
	if(argc >=2)
		on = atoi(argv[1]);
	initGPIO();
	restartUSB(on);
	return 0;
}
