/*************************************************************************
	> File Name: time_main.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Fri 28 Nov 2014 03:57:02 PM CST
 ************************************************************************/

#include<stdio.h>
#include <time.h>
#include <syslog.h>

int main(int argc,char *argv[])
{

	//get seconds from 1970year
	time_t now;
	while(1)
	{
		now = time(NULL);
		syslog(LOG_INFO,"now:%ld\n", now);
		usleep(500*1000);
	}
	return 0;
}
