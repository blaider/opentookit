#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>

pid_t killpid;
void sig_alarm(int intno)
{
	kill(killpid,SIGINT);
	exit(0);
}
int main(int argc,char *argv[])
{
	int seconds;
	daemon(0,0);
	killpid = strtoul(argv[1],NULL,10);
	seconds = strtoul(argv[2],NULL,10);
	signal(SIGALRM,sig_alarm);
	alarm(seconds);
	while(!kill(killpid,0))
	{
		sleep(1);
	}
	return 0;
}
