/*************************************************************************
	> File Name: sramnull.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 29 Nov 2017 09:58:18 AM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#define SRAM_DEV 		"/dev/sram"

int main ( int argc, char* argv[] )
{
	char buff[30];
	int i=0;
	int sram_fd = open(SRAM_DEV, O_RDWR);
	if( sram_fd == -1)
		return -1;

	for(;;)
	{
//		write(sram_fd, NULL, 10);
		lseek(sram_fd, 0, SEEK_SET);
		read(sram_fd, buff, 10);
		printf("%d----\n",i++);
		sleep(1);
	}
}
