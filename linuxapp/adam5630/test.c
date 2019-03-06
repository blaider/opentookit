/*************************************************************************
	> File Name: test.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 14 Mar 2018 11:18:36 AM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
	char buff[32];
	FILE * fp;
	fp = fopen("/sys/bus/iio/devices/iio:device0/in_voltage5_raw","r");
	if(fp == NULL)
	{
		printf("open failed \n");
		return -1;
	}

	while(1)
	{
		size_t n = 0;
		memset(buff,0,sizeof(buff));
		fseek(fp,0L,SEEK_SET);
		n = fread(buff,1,sizeof(buff),fp);
		printf("%d,%s\n",n,buff);
		sleep(1);
	}
	return 0;

}
