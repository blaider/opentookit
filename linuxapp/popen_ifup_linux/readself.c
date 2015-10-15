/*************************************************************************
 > File Name: readself.c
 > Author: suchao.wang
 > Mail: suchao.wang@advantech.com.cn
 > Created Time: Fri 05 Dec 2014 06:51:33 PM CST
 ************************************************************************/

#include<stdio.h>
#include <limits.h>
#include <unistd.h>

#define PROC_NET_WIRELESS	"ifup eth1 2>&1 | grep  config"
int readWirelessSignal(const char *ifname)
{
	int ret = 0;
	FILE * f = fopen(PROC_NET_WIRELESS, "r");
	char buf[256];
	char * bp;
	int t;

	if (f == NULL)
	{
		perror(PROC_NET_WIRELESS);
		return -1;
	}
	/* Loop on all devices */
	while (fgets(buf, 255, f))
	{
		bp = buf;
		printf("%s:%s\n",__func__,buf);
	}
	fclose(f);
	return ret;

}
int main(int argc, char *argv[])
{
	readWirelessSignal("wlan0");
	return 0;
}
