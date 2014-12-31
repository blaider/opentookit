/*************************************************************************
	> File Name: timerkill.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Tue 23 Dec 2014 07:18:38 PM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <syslog.h>

pid_t killpid;
void sig_alarm(int no)
{
	kill(killpid,SIGINT);
	exit(0);
}

int main(int argc,char *argv[])
{
	int seconds;
	daemon(0,0);
	syslog(LOG_INFO,"%s %s %s",argv[1],argv[2],argv[3]);
	killpid = strtoul(argv[1],NULL,10);
	seconds = strtoul(argv[2],NULL,10);
	signal(SIGALRM,sig_alarm);
	alarm(seconds);
	while(1);
	return 0;
}
