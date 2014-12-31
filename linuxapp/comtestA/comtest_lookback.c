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
// File:   	 main.c
// Author:  suchao.wang
// Created: Nov 5, 2014
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <syslog.h>
#include <signal.h>

#define APP_VERSION "1.0.2"
#define MAX_RUN_SECONDS 20
struct BaudRate
{
	int speed;
	int bitmap;
};
struct BaudRate baudlist[] =
{
//{ 50, B50 },
//{ 75, B75 },
//{ 110, B110 },
//{ 134, B134 },
//{ 150, B150 },
//{ 200, B200 },
//{ 300, B300 },
//{ 600, B600 },
		{ 1200, B1200 },
		{ 1800, B1800 },
		{ 2400, B2400 },
		{ 4800, B4800 },
		{ 9600, B9600 },
		{ 19200, B19200 },
		{ 38400, B38400 },
		{ 57600, B57600 },
		{ 115200, B115200 },
		{ 230400, B230400 },
		{ 460800, B460800 },
		{ 500000, B500000 },
		{ 576000, B576000 },
		{ 921600, B921600 },
//{ 1000000, B1000000 },
//{ 1152000, B1152000 },
//{ 1500000, B1500000 },
//{ 2000000, B2000000 },
//{ 2500000, B2500000 },
//{ 3000000, B3000000 },
//{ 3500000, B3500000 },
//{ 4000000, B4000000 },
		};
int comDatabits[] =
{ 5, 6, 7, 8 };
int comStopbits[] =
{ 1, 2 };
int comParity[] =
{ 'n', 'o', 'e' };
char g_comDevicesName[256][256];
int g_comNums;
char g_logfile[256] = {0};
typedef struct devError{
	char devName[50];
	char showName[50];
	long err;
}DEVERROR;
DEVERROR devs[]=
{
{"/dev/ttyS0","COM1",0},
{"/dev/ttyS1","COM2",0},
{"/dev/ttyAP0","COM3",0},
{"/dev/ttyAP1","COM4",0},
{"/dev/ttyAP2","COM5",0},
{"/dev/ttyAP3","COM6",0},
{"/dev/ttyAP4","COM7",0},
{"/dev/ttyAP5","COM8",0},
{"/dev/ttyAP6","COM9",0},
{"/dev/ttyAP7","COM10",0},
};


int set_com(int fd, int speed, int databits, int stopbits, int parity)
{
	int i;
	struct termios opt;

	if (tcgetattr(fd, &opt) != 0)
	{
		perror("get attr failed!\n");
		return -1;
	}

	for (i = 0; i < sizeof(baudlist) / sizeof(baudlist[0]); i++)
	{
		struct BaudRate *rate = &baudlist[i];
		if (speed == rate->speed)
		{
			cfsetispeed(&opt, rate->bitmap);
			cfsetospeed(&opt, rate->bitmap);
			break;
		}
	}
//	//修改控制模式，保证程序不会占用串口
//	opt.c_cflag |= CLOCAL;
//	//修改控制模式，使得能够从串口中读取输入数据
//	opt.c_cflag |= CREAD;

	opt.c_cflag &= ~CSIZE;
	switch (databits)
	{
	case 5:
		opt.c_cflag |= CS5;
		break;
	case 6:
		opt.c_cflag |= CS6;
		break;
	case 7:
		opt.c_cflag |= CS7;
		break;
	case 8:
		opt.c_cflag |= CS8;
		break;
	default:
		printf("Unsupported data size\n");
		return -1;
	}

	switch (parity)
	{
	case 'n':
	case 'N':
		opt.c_cflag &= ~PARENB;
		opt.c_iflag &= ~INPCK;
		break;
	case 'o':
	case 'O':
		opt.c_cflag |= (PARODD | PARENB);
		opt.c_iflag |= INPCK;
		break;
	case 'e':
	case 'E':
		opt.c_cflag |= PARENB;
		opt.c_cflag &= ~PARODD;
		opt.c_iflag |= INPCK;
		break;
	default:
		printf("Unsupported parity\n");
		return -1;
	}

	switch (stopbits)
	{
	case 1:
		opt.c_cflag &= ~CSTOPB;
		break;
	case 2:
		opt.c_cflag |= CSTOPB;
		break;
	default:
		printf("Unsupported stop bits\n");
		return -1;
	}

	opt.c_iflag &= ~(IXON | IXOFF | IXANY | BRKINT | ICRNL | INPCK | ISTRIP);
	opt.c_lflag &= ~(ICANON | ECHO | ECHOE | IEXTEN | ISIG);
	opt.c_oflag &= ~OPOST;
	opt.c_cc[VTIME] = 10;
	opt.c_cc[VMIN] = 0;

	tcflush(fd, TCIOFLUSH);
	if (tcsetattr(fd, TCSANOW, &opt) != 0)
	{
		perror("set attr failed!\n");
		return -1;
	}
	return 0;
}

int OpenDev(char* Dev)
{
	int fd = open(Dev, O_RDWR | O_NOCTTY);
	if (-1 == fd)
	{
		perror("open failed!\n");
		return -1;
	}
	else
		return fd;
}

int openDevice(const char* Dev, int speed, int databits, int stopbits,
		int parity)
{
	int fd;
	fd = open(Dev, O_RDWR | O_NOCTTY);
	if (-1 == fd)
	{
		perror("open failed!\n");
		return -1;
	}

	if (set_com(fd, speed, databits, stopbits, parity) != 0)
	{
		printf("Set Com Error\n");
		return -1;
	}

	return fd;
}
int isCom(const char * name)
{
	int fd;
	int ret = 0;
	struct termios opt;
	fd = open(name, O_RDWR | O_NOCTTY);
	if (-1 == fd)
	{
		return 0;;
	}

	if (tcgetattr(fd, &opt) == 0)
	{
		ret = 1;
	}
	close(fd);
	return ret;
}
#define MSG1 "ADVANTEC"
#define MSG2 "advantec"
int selfTest(DEVERROR dev)
{
	char buf[128];
	char devName[50];
	int fd;
	int ret;
	int strLen;

	strcpy(devName, dev.devName);
	fd = openDevice(devName, 9600, 8, 1, 'n');
	if (fd <= 0)
	{
		printf("open device %s failed!\n", dev.showName);
		return -1;
	}
	strLen = strlen(MSG1);
	ret = write(fd, MSG1, strLen);
//	if (ret != strLen)
//		printf("%s write '%s' failed--------\n", dev.showName, MSG1);
	//usleep(1000*1000/(96000*strLen));
	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, strlen(MSG1));
//	if (ret != strLen)
//		printf("%s read '%s' failed,%d,%s--------\n", dev.showName, MSG1,ret,buf);

	if (strcmp(buf, MSG1))
	{
		printf("Test %s fail\n", dev.showName);
	}else
		printf("Test %s OK\n", dev.showName);

	close(fd);

	return 0;
}
int main(int argc, char *argv[])
{
	size_t i;
	for(i=0;i<sizeof(devs)/sizeof(devs[0]);i++)
		selfTest(devs[i]);
	return 0;
}

