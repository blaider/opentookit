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

pid_t selfpid;
void sig_alarm(int intno)
{
	kill(selfpid,SIGINT);

}


void sig_int(int intno)
{
	printf("i will exit1\n");
	printf("i will exit2\n");
	printf("i will exit3\n");
	printf("i will exit4\n");
	printf("i will exit5\n");
	system("./ping.sh");
	exit(0);
}
int main(int argc,char *argv[])
{

	signal(SIGINT,sig_int);
//	signal(SIGALRM,sig_alarm);
//	alarm(10);
	char buf[1024];
	sprintf(buf,"./timerkill %d %d %s",getpid(),10);
	system(buf);
	while(1)
	{
		sleep(1);
	}
	return 0;
}
