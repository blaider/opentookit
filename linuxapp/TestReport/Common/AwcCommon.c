// #############################################################################
// *****************************************************************************
//                  Copyright (c) 2014, Advantech Automation Corp.
//      THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY
//               INFORMATION WHICH IS THE PROPERTY OF ADVANTECH AUTOMATION CORP.
//
//    ANY DISCLOSURE, USE, OR REPRODUCTION, WITHOUT WRITTEN AUTHORIZATION FROM
//               ADVANTECH AUTOMATION CORP., IS STRICTLY PROHIBITED.
// *****************************************************************************
// #############################################################################
//
// File:   	 NetdeviceInfo.cpp
// Author:  suchao.wang
// Created: Jul 18, 2014
//
// Description: get or set info for netdevice.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <termios.h>
#include <ctype.h>
#include "AwcCommon.h"
#include "Log.h"
#include <errno.h>
#include <sys/types.h>

#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#ifndef DEFFILEMODE
#define DEFFILEMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)/* 0666*/
#endif

struct in_addr GetIP( char const * if_name )
{
	int sockfd;
	struct ifreq ifr;
	int rc;
	struct in_addr addr;
	in_addr_t addrt;
	bzero(&addr,sizeof(struct in_addr));

	sockfd = socket( AF_INET, SOCK_STREAM, 0);
	if (0 == sockfd)
	{
		return addr;
	}

	bzero(&ifr, sizeof(ifr));
	strncpy(ifr.ifr_name, if_name, sizeof(ifr.ifr_name));

	rc = ioctl(sockfd, SIOCGIFADDR, &ifr);

	close(sockfd);

	if (rc != 0)
	{
		return addr;
	}

	if (ifr.ifr_addr.sa_family != AF_INET)
	{
		return addr;
	}

//	LOGEX("NetdeviceInfo::GetIP,%08x",((struct sockaddr_in*) &ifr.ifr_addr)->sin_addr.s_addr);

	addrt = ((struct sockaddr_in*) &ifr.ifr_addr)->sin_addr.s_addr;
	memcpy(&addr,&addrt,sizeof(addrt));
	return addr;

}

void skipline(FILE *f)
{
  int ch;
  do {
    ch = getc(f);
  } while ( ch != '\n' && ch != EOF );
}
int NetIsExist(const char *ifname)
{
	int ret = 0;
	FILE *pnd;
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
//		printf("find:%s\n",devName);
		if(!strcmp(devName, ifname))
		{
			ret = 1;
			break;
		}
	}
	fclose(pnd);
	return ret;

}

void gpio_set(void * addr,unsigned int offset)
{
	volatile unsigned int *gpio_dout = (volatile unsigned int *)((volatile unsigned char *)addr + 0x13c);
    unsigned int reg = *((volatile unsigned int *)gpio_dout);
    reg |= (1 << offset);
	*((volatile unsigned int *)gpio_dout) = reg;

//	 LOGEX("%s:%x\n",__func__,*(volatile unsigned int *) addr);
	 return;
}
void gpio_clear(void * addr,unsigned int offset)
{
	volatile unsigned int *gpio_dout = (volatile unsigned int *)((volatile unsigned char *)addr + 0x13c);
    unsigned int reg = *((volatile unsigned int *)gpio_dout);
    reg &= ~(1 << offset);
	*((volatile unsigned int *)gpio_dout) = reg;

//	 LOGEX("%s:%x\n",__func__,*(volatile unsigned int *) addr);
	 return;
}
struct gpioinfo
{
	off_t phyAddress;
	int regOffset;
};
#define GPIO0_BASE 0x44E07000
#define GPIO3_BASE 0x481AE000
int restartUSB(int usbBusNo)
{
	struct gpioinfo sinfo[] = {{GPIO0_BASE,4},{GPIO3_BASE,15}};

	if(!usbBusNo || usbBusNo > sizeof(sinfo)/sizeof(sinfo[0]))
		return 0;

	struct gpioinfo *pinfo = &sinfo[usbBusNo-1];
	int mapfd = -1;
	void * viraddr = NULL;
	if (mapfd < 0)
	{
		mapfd = open("/dev/mem", O_RDWR | O_SYNC); //鎵撳紑mem鏂囦欢锛岃闂洿鎺ュ湴鍧€
		if (mapfd == -1)
		{
			return -1;
		}
	}
	viraddr = mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, mapfd,
			pinfo->phyAddress);

	volatile unsigned int *gpio_oe =
			(volatile unsigned int *) ((volatile unsigned char *) viraddr
					+ 0x134);
	unsigned int reg = *((volatile unsigned int *) gpio_oe);
	reg &= ~(1 << pinfo->regOffset);
	*((volatile unsigned int *) gpio_oe) = reg;

	gpio_clear(viraddr, pinfo->regOffset);
	usleep(100 * 1000);
	gpio_set(viraddr, pinfo->regOffset);

	close(mapfd);
	return 0;

}
int usb_detect (const char *usbName)
{
	int ret = 0;
	FILE * fp;
	char line_str[ 1024 * 64 ];

	fp = popen( "lsusb", "r" );
	if ( NULL == fp )
	{
		return 0;
	}
	memset( line_str, 0,sizeof( line_str ) );
//	printf("find device:%s\n",conn->usbName);
	while ( fgets( line_str, sizeof( line_str ), fp ) != NULL )
	{

		if ( strstr( line_str, usbName ) != NULL )
		{
			ret = 1;
			break;
		}
	}
	pclose( fp );

	return ret;
}

int find_usb_bus_no (const char *usbName,int *usbBusNo)
{
	int ret = 0;
	FILE * fp;
	char line_str[ 1024 * 64 ];

	fp = popen( "lsusb | sort", "r" );
	if ( NULL == fp )
	{
		return 0;
	}
	memset( line_str, 0,sizeof( line_str ) );
//	printf("find device:%s\n",conn->usbName);
	while ( fgets( line_str, sizeof( line_str ), fp ) != NULL )
	{

		if ( strstr( line_str, usbName ) != NULL )
		{
			sscanf(line_str,"%*s %d",usbBusNo);
			ret = 1;
			break;
		}
	}
	pclose( fp );

	return ret;
}

void serial_init(int fd)
{
	struct termios options;
	tcgetattr(fd, &options);
	options.c_cflag |= ( CLOCAL | CREAD);
	options.c_cflag &= ~CSIZE;
	options.c_cflag &= ~CRTSCTS;
	options.c_cflag |= CS8;
	options.c_cflag &= ~PARENB;
	options.c_iflag &= ~INPCK;
	options.c_cflag &= ~CSTOPB;
	options.c_oflag = 0;
	options.c_lflag = 0;

	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	tcsetattr(fd, TCSANOW, &options);
}


#define DIRS "/dev/serial/by-id/"
#define PRE_LINK "../../"

int usbLable_detect(char *usbLable)
{
	FILE * fd;
	char line[4096];
	char name[4096];
	sprintf(name,"ls %susb-%s*",DIRS,usbLable);
	fd = popen(name, "r");
	if(fd == NULL)
	{
		perror(DIRS);
		return -1;
	}

	int inx = 0;

	while (fgets(line, sizeof(line), fd))
	{
//		printf("read:%s",line);
		inx++;
	}
	pclose(fd);
	return inx;
}



int get_gprs_config_node_name(char *usbLable,int ttyPortNo,char *ttyPort)
{
	int ret = 0;
	FILE * fd;
	char line[4096];
	char name[4096],linkName[4096];
	sprintf(name,"ls %susb-%s*",DIRS,usbLable);
	fd = popen(name, "r");
	if(fd == NULL)
	{
		perror(DIRS);
		return -1;
	}

	int inx = 0;

	while (fgets(line, sizeof(line), fd))
	{
//		printf("read:%s",line);
		sprintf(name,"%s",line);
		if(name[strlen(name)-1] == 0xa || name[strlen(name)-1] == 0xd)
			name[strlen(name)-1] = 0x0;
		memset(linkName,0,sizeof(linkName));
		int linkSize = readlink(name,linkName,sizeof(linkName));
		if(-1 == linkSize)
		{
//			perror("readlink");
			ret = -1;
			continue;
		}
//		LOGEX("%s %s %s\n",name,(linkSize > 0)?" ->":"",linkName);
		if(ttyPortNo == inx )
		{
			if (strncmp(linkName, PRE_LINK, strlen(PRE_LINK)) == 0)
			{
				sprintf(ttyPort, "/dev/%s", linkName + strlen(PRE_LINK));
				ret = 1;
			}
		}
		inx++;
	}
	pclose(fd);
	if(access(ttyPort,F_OK))
		return -1;
	return ret;
}

int checkProcess(const char *name)
{
	char buff[80];
	pid_t pid = 0;
	char *ret;

	sprintf(buff, "pidof %s", name);
	FILE *pidreader = popen(buff, "r");
	memset(buff, 0, 80);

	ret = fgets(buff, 79, pidreader);
	pclose(pidreader);

	ret = strtok(buff, " ");
	while (ret != NULL)
	{
//		int status;
		pid = atoi(ret);
		ret =strtok(NULL," ");
		break;
	}
	return pid;
}


int system_ping(const char *ifname,PINGURL * urls)
{
	if(!urls->lastPingTime)
		urls->lastPingTime = time(NULL);

	int ret = 0;
	unsigned int i=0;
	for(i=0;i<sizeof(urls->url)/sizeof(urls->url[0]);i++)
	{
		if(strlen(urls->url[i]) < 1)
			continue;
		char cmd[2056];
		sprintf(cmd,"ping %s -c 1 -W 10 -I %s",urls->url[i],ifname);
		printf("%s:%s\n",__func__,cmd);
		ret = system(cmd);
		printf("%s:ret=%d\n",__func__,ret);
		if(!ret)
		{
			urls->lastPingTime = time(NULL);
			break;
		}
		printf("%s:%s failed:%d\n",__func__,cmd,i);
	}
	return !ret;
}



int traffic_monitor(const char *ifName,TRAFFICDETAIL *tdetail)
{
//	printf("%s:mtime:%ld,interval:%d,left=%ld\n",__func__,tdetail.mtime,gprsSilenceMinutes*60,gprsSilenceMinutes*60+tdetail.mtime-time(NULL));
//	TRAFFICDETAIL *tdetail = &conn->tdetail;
	if(!tdetail->trafficMinutes)
		return 1;

	if(!tdetail->mtime)
		tdetail->mtime = time(NULL);
	int ret = 1;
	FILE *pnd;
	struct ifinfo
	{
		char name[8];
		unsigned int r_bytes, r_pkt, r_err, r_drop, r_fifo, r_frame;
		unsigned int r_compr, r_mcast;
		unsigned int x_bytes, x_pkt, x_err, x_drop, x_fifo, x_coll;
		unsigned int x_carrier, x_compr;
	} ifc;

	pnd = fopen("/proc/net/dev", "r");
	if (!pnd)
	{
		fprintf(stderr, "%s: /proc/net/dev: %s", "aa", strerror(errno));
		exit(1);
	}

	/* Skip header */
	skipline(pnd);
	skipline(pnd);

	/* Get interface info */
	do
	{
		memset(ifc.name, 0, sizeof(ifc.name));
		int ret = fscanf(pnd,
				" %6[^:]:%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u ",
				ifc.name, &ifc.r_bytes, &ifc.r_pkt, &ifc.r_err, &ifc.r_drop,
				&ifc.r_fifo, &ifc.r_frame, &ifc.r_compr, &ifc.r_mcast,
				&ifc.x_bytes, &ifc.x_pkt, &ifc.x_err, &ifc.x_drop,
				&ifc.x_fifo, &ifc.x_coll, &ifc.x_carrier, &ifc.x_compr);
		if (ret != 17)
		{
			break;
		}
	} while (strcmp(ifc.name, ifName));
	fclose(pnd);
	if (!strcmp(ifc.name, ifName))
	{
		if((ifc.r_bytes != tdetail->rxbytes) && (ifc.x_bytes != tdetail->rxbytes))
		{
			tdetail->mtime = time(NULL);
			tdetail->rxbytes = ifc.r_bytes;
			tdetail->rxbytes = ifc.x_bytes;
			ret = 1;
		}else if(time(NULL) - tdetail->mtime  > tdetail->trafficMinutes*60)
		{
			ret = 0;
		}else{
			printf("%s:%s,left %ld seconds\n",__func__,ifName,tdetail->trafficMinutes*60 + tdetail->mtime - time(NULL) );
		}
	}else{
		ret = 0;
	}
  return ret;
}


int get_program_path(char *path)
{
	int i = 0;
	if(path == NULL)
		return -1;
	char szPath[PATH_MAX];
	size_t rslt = readlink("/proc/self/exe", szPath, sizeof(szPath) - 1);
	if (rslt < 0 || (rslt >= sizeof(szPath) - 1)) {
		return -1;
	}
	szPath[rslt] = '\0';
	for (i = rslt; i >= 0; i--) {
		if (szPath[i] == '/') {
			szPath[i + 1] = '\0';
			break;
		}
	}
	strcpy(path,szPath);
	return 0;
}

