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

const char gprs_USBDeviceName[] = "ZTE WCDMA Technologies MSM" ;

static int usb_detect ()
{
	int ret = 0;
	FILE * fp;
	char line_str[ 1024 * 64 ];
	int bus,device;

	fp = popen( "lsusb", "r" );
	if ( NULL == fp )
	{
		return 0;
	}
	memset( line_str, 0,sizeof( line_str ) );
//	printf("find device:%s\n",gprs_USBDeviceName);
	while ( fgets( line_str, sizeof( line_str ), fp ) != NULL )
	{

		sscanf(line_str,"%*s %d %*s %d",&bus,&device);
		printf("bus=%d,device=%d\n",bus,device);
		if ( strstr( line_str, gprs_USBDeviceName ) != NULL )
		{
			sscanf(line_str,"%*s %d %*s %d",&bus,&device);
			printf("find bus=%d,device=%d\n",bus,device);
			ret = 1;
//			break;
		}
	}
	pclose( fp );

	return ret;
}
int main(int argc, char *argv[])
{
	usb_detect();
}
