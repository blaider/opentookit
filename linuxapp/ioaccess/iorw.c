/*************************************************************************
	> File Name: iorw.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 29 Nov 2017 11:39:03 AM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define IO_BASE (0xd100)
#define IO_LEN 256

int main(int argc,char *argv[])
{
	uid_t uid;
	int i = 0;
	uid = getuid();
	if(uid != 0)
	{
		printf("please run as root\n");
		return -1;
	}
	int ret = ioperm( IO_BASE, IO_LEN, 1);
	if(ret)
	{
		perror("ioperm");
	}

	if(argc == 2)
	{
		int offset = strtoul(argv[1],NULL,0);
		if(offset > IO_LEN)
		{
			printf("offset[%d] is bigger than %d\n",offset,IO_LEN);
			return -1;
		}
		unsigned char value = inb(IO_BASE + offset);
		printf("Get addr:0x%02x[%d] = 0x%02x [%d]\n",offset,offset,value,value);
	}else if(argc == 3)
	{
		int offset = strtoul(argv[1],NULL,0);
		unsigned char value =  strtoul(argv[2],NULL,0);
		if(offset > IO_LEN)
		{
			printf("offset[%d] is bigger than %d\n",offset,IO_LEN);
			return -1;
		}
		outb(value,IO_BASE + offset);
		printf("Set addr:0x%02x[%d] = 0x%02x [%d]\n",offset,offset,value,value);
	}

}
