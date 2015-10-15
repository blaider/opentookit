/*************************************************************************
	> File Name: lan_traffic.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Tue 02 Dec 2014 02:46:22 PM CST
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/reboot.h>

void skipline(FILE * f)
{
	int ch;
	do
	{
		ch = getc(f);
	} while (ch != '\n' && ch != EOF);
}
int netcard_isexist(const char *ifname)
{
	int ret = 0;
	FILE * pnd;
	char devName[8];
	pnd = fopen("/proc/net/dev", "r");
	if (!pnd)
	{
		return 0;
	}
	/* Skip header */
	skipline(pnd);
	skipline(pnd);
	/* Get interface info */
	while (!feof(pnd))
	{
		memset(devName, 0, sizeof(devName));
		fscanf(pnd, " %6[^:]:%*[^\n]", devName);
//              printf("find:%s\n",devName);
		if (!strcmp(devName, ifname))
		{
			ret = 1;
			break;
		}
	}
	fclose(pnd);
	return ret;
}
int main(int argc, char *argv[])
{
	daemon(0,0);
	sleep(40);
	char command[512];
	sprintf(command,"wan.sh cmnet /dev/ttyUSB1");

	while (1)
	{
		syslog(LOG_INFO, "%s", command);
		system(command);

		sleep(10);
		if (netcard_isexist("ppp0"))
		{
			syslog(LOG_INFO, "find ppp0");
			sleep(20);
			system("reboot");
			reboot(RB_AUTOBOOT);
		}else{
			syslog(LOG_INFO, "not found ppp0");
		}
	}
}
