/*************************************************************************
	> File Name: mem_alloc.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 03 Dec 2014 06:10:58 PM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

static struct tm local_tm;
static time_t now;

static void *test_time(void *arg)
{
	while(1)
	{
		time(&now);
		tzset();
		localtime_r(&now,&local_tm);
		sleep(1);
	}
}

static void* test_tnd(void*arg)
{
	while(1)
		system("lsusb");
}

int main(int argc,char *argv[])
{
	char * buf;
	int size = 10;
	printf("pid:%d\n",getpid());
	for(;;)
	{
		buf = malloc(size*1024*1024);
		if(buf == NULL)
			break;
		free(buf);
		size += 10;
	}
	int msize = (size-40)*1024*1024;
	buf = malloc(msize);
	printf("%dM\n",size-40);
//	memset(buf, 0xa5, msize);
	pthread_t m_hThread;
	pthread_create(&m_hThread,NULL,test_time,NULL);
//	pthread_create(&m_hThread,NULL,test_time,NULL);
	while (1)
	{
		sleep(1);
//		memset(buf, 0x5a, msize);
//		memset(buf, 0xa5, msize);
	}
	return 0;
}
