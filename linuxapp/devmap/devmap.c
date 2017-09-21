/*************************************************************************
	> File Name: devmap.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 30 Nov 2016 02:22:29 PM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#define GPIO3_BASE 0x481AE000
#define GPIO2_BASE 0x481AC000
#define GPIO1_BASE 0x4804C000
#define GPIO0_BASE 0x44E07000

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)


#define CM_PER_BASE 0x44E00000
#define GPIO1_CLK_CONFIG_OFFSET 0xAC
#define GPIO2_CLK_CONFIG_OFFSET 0xB0

int main(int argc,char *argv[])
{
	int mapfd = open("/dev/mem", O_RDWR | O_SYNC); //鎵撳紑mem鏂囦欢锛岃闂洿鎺ュ湴鍧€
	if (mapfd == -1)
	{
		printf("devmap fail!\n");
		return -1;
	}
	void *virclock = mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED,	mapfd, CM_PER_BASE & ~MAP_MASK);

	printf("clock:%x\n",*(volatile unsigned int *)(virclock + GPIO2_CLK_CONFIG_OFFSET));
//	*(volatile unsigned int *)(virclock + GPIO1_CLK_CONFIG_OFFSET) = 0x02;
//设置(CM_PER_BASE+GPIO1_CLK_CONFIG_OFFSET) = 0x02
	void *viraddr=mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED,	mapfd, GPIO2_BASE & ~MAP_MASK);
	viraddr += GPIO2_BASE & MAP_MASK;

	unsigned int oe = *(volatile unsigned int *)(viraddr + 0x13c);
	printf("oe:%x,%d\n",oe,!!(oe & (1<<9)));
	return 0;
}
