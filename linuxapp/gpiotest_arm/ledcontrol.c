/*************************************************************************
 > File Name: ledcontrol.c
 > Author: suchao.wang
 > Mail: suchao.wang@advantech.com.cn
 > Created Time: Sat 28 Feb 2015 02:39:00 PM CST
 ************************************************************************/

#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define GPIO3_BASE 0x481AE000
#define GPIO1_BASE 0x4804C000

void gpio_set(void * addr,unsigned int offset)
{
	 *(volatile unsigned int *) addr |= 1 << offset;
	 return;
}
void gpio_clear(void * addr,unsigned int offset)
{
	 *(volatile unsigned int *) addr &= ~(1 << offset);
	 return;
}
int main(int argc, char **argv)
{
	char *FPGA_PIN_base;
	char *base1,base2;
	int data = 0;

	int fd;
	fd = open("/dev/mem", O_RDWR | O_SYNC); //鎵撳紑mem鏂囦欢锛岃闂洿鎺ュ湴鍧€
	if (fd == -1)
	{
		printf("ERROR OF OPEN mem!\n");
		exit(-1);
	}
	printf("OPEN the mem!\n");
////////////////////////////////////////////////////////////////////////////////////////////
	FPGA_PIN_base = (char *) mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED,
			fd, GPIO3_BASE);
	base1 = (char *) mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED,
				fd, GPIO1_BASE);
	base2 = (char *) mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED,
					fd, 0x44e109e4);

	printf("FPGA_PIN=0x%x.PINMUX=0x%x\n", FPGA_PIN_base, data);
///////////////////////////////////////////////////////////////////////////////////////////////////
	data = *(volatile unsigned int *) (FPGA_PIN_base + 0x13C);
	printf("FPGA_PIN=0x%x.PINMUX5=0x%x\n", FPGA_PIN_base + 0x13C, data);
	printf("FPGA_PIN=0x%x.PINMUX5=0x%x\n", FPGA_PIN_base + 0x13C,
			*(volatile unsigned int *) (FPGA_PIN_base + 0x13C));
	data = *(volatile unsigned int *) (base2);
	printf("base2:0x%x\n",data);
	data = *(volatile unsigned int *) (base2+4);
	printf("base2:0x%x\n",data);
	gpio_clear(FPGA_PIN_base + 0x13C,7);
	gpio_clear(base1 + 0x13C,9);
	while(1)
	{
		gpio_clear(FPGA_PIN_base + 0x13C,8);
		gpio_clear(FPGA_PIN_base + 0x13C,7);
		gpio_clear(base1 + 0x13C,9);
		sleep(1);
		gpio_set(FPGA_PIN_base + 0x13C,8);
		gpio_set(FPGA_PIN_base + 0x13C,7);
		gpio_set(base1 + 0x13C,9);
		sleep(1);
	}

//	*(volatile unsigned int *) (FPGA_PIN_base + 0x13C) = 0x23; //PINMUX5 0x01C14000+0x13C=0x01C14134
//
//	data = *(volatile unsigned int *) (FPGA_PIN_base + 0x13C);
//	printf("FPGA_PIN=0x%x.PINMUX5=0x%x\n", FPGA_PIN_base + 0x13C, data);
//	printf("FPGA_PIN=0x%x.PINMUX5=0x%x\n", FPGA_PIN_base + 0x13C,
//			*(volatile unsigned int *) (FPGA_PIN_base + 0x13C));

	return 0;
}

