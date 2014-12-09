/*************************************************************************
	> File Name: time_main.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Fri 28 Nov 2014 03:57:02 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void sig_int(int intno)
{
	printf("i will exit1\n");
	printf("i will exit2\n");
	printf("i will exit3\n");
	printf("i will exit4\n");
	printf("i will exit5\n");
	exit(0);
}
int main(int argc,char *argv[])
{

	signal(SIGINT,sig_int);
	signal(SIGALRM,sig_int);
	alarm(10);
	while(1)
	{
		sleep(1);
	}
	return 0;
}
