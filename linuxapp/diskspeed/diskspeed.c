/*************************************************************************
	> File Name: diskspeed.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Thu 05 Jul 2018 02:18:42 PM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>


// 返回自系统开机以来的毫秒数（tick）
unsigned long GetTickCount()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (ts.tv_sec);
}

int main(int argc,char *argv[])
{
	int speed = 8*1024*1024;//4M/s
	int size=1024*1024*1024;//1G
	unsigned long last_tick = GetTickCount();
	int quta_size = speed/4;

	FILE *fp=fopen("test.db","w");
	if(fp == NULL)
	{
		printf("file open failed\n");
		return 0;
	}
	char *buf=(char *)malloc(quta_size);
	int total_write = 0;
	last_tick = GetTickCount();
	while(total_write < size)
	{
		printf("%d\n",total_write/1024/1024);
		fwrite(buf,1,quta_size,fp);
		fsync(fp);
		system("sync;");
		total_write+=quta_size;
		int diff_tick = GetTickCount() - last_tick;
		if(diff_tick < 250)
		{
			usleep((250-diff_tick)*1000);
			last_tick = GetTickCount();
		}else{
			last_tick = GetTickCount();
		}
	}
	fclose(fp);
	return 0;

}
