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

void LOG(const char* ms, ...)
{
	char wzLog[1024] =
	{ 0 };
	char buffer[1024] =
	{ 0 };
	time_t now;
	struct tm *local;
	FILE* file;

	va_list args;
	va_start(args, ms);
	vsprintf(wzLog, ms, args);
	va_end(args);

	time(&now);
	local = localtime(&now);
	sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d %s", local->tm_year + 1900,
			1 + local->tm_mon, local->tm_mday, local->tm_hour, local->tm_min,
			local->tm_sec, wzLog);
	printf("%s",buffer);
	file = fopen("testResut.log", "a+");
	fwrite(buffer, 1, strlen(buffer), file);
	fclose(file);

//	syslog(LOG_INFO,wzLog);
	return;
}

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
	opt.c_cc[VTIME] = 20;
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
#define MSG1 "HELLO ADVANTECH"
#define MSG2 "hello advantech"
int selfTest(const char *comName)
{
	char buf[128];
	char devName[50];
	int fd;
	int ret;
	int strLen;

	strcpy(devName, comName);
	fd = openDevice(devName, 115200, 8, 1, 'n');
	if (fd <= 0)
	{
		LOG("open device %s failed!\n", devName);
		return -1;
	}
	strLen = strlen(MSG1);
	ret = write(fd, MSG1, strLen);
	if (ret != strLen)
		LOG("%s write '%s' failed--------\n", devName, MSG1);
	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, strlen(MSG1));
	if (ret != strLen)
		LOG("%s read '%s' failed--------\n", devName, MSG1);

	if (strcmp(buf, MSG1))
		LOG("%s test %s failed--------\n", devName, MSG1);
	else
		printf("%s test %s success\n", devName, MSG1);

	strLen = strlen(MSG2);
	ret = write(fd, MSG2, strLen);
	if (ret != strLen)
		LOG("%s write '%s' failed========\n", devName, MSG2);
	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, strlen(MSG2));
	if (ret != strLen)
		LOG("%s read '%s' failed========\n", devName, MSG2);

	if (strcmp(buf, MSG2))
		LOG("%s test %s failed========\n", devName, MSG2);
	else
		printf("%s test %s success\n", devName, MSG2);
	close(fd);

	return 0;
}
void* allSelfTestThread(void* arg)
{
	int no = (int) arg;
	char name[50];
	sprintf(name, "/dev/ttyAP%u", no);
	while (1)
	{
		selfTest(name);
		sleep(1);
	}
	return NULL;
}

#define COM_NUMS 1
int comTest()
{
	pthread_t thread[COM_NUMS];
	int i;
	for (i = 0; i < COM_NUMS; i++)
	{
		pthread_create(&thread[i], NULL, allSelfTestThread, (void*) i);
	}
	return 0;
}
void *comTestThread(void *arg)
{
	for (;;)
	{
		selfTest("/dev/ttyS0");
		selfTest("/dev/ttyS1");
		selfTest("/dev/ttyAP0");
		selfTest("/dev/ttyAP1");
		selfTest("/dev/ttyAP2");
		selfTest("/dev/ttyAP3");
		selfTest("/dev/ttyAP4");
		selfTest("/dev/ttyAP5");
		selfTest("/dev/ttyAP6");
		selfTest("/dev/ttyAP7");
	}
	return NULL;
}

int get_system_memory_size(long *total)
{

	long ma;
	FILE * fd;
	char line[100];
	char name[100];
	if (total == NULL)
			return -1;

	fd = fopen("/proc/meminfo", "r");
	if (fd == NULL)
	{
		perror("/proc/meminfo");
		return -1;
	}
	while (fgets(line, sizeof(line), fd))
	{
		sscanf(line, "%[^:] %*[^ ]  %ld %*[^\n ]", name, &ma);
		if (strcmp(name, "MemTotal") == 0)
		{
			*total = ma * 1024;
			break;
		}
	}
	fclose(fd);
	return 0;
}
int get_system_memory_free(long *free)
{

	long ma;
	FILE * fd;
	char line[100];
	char name[100];
	if (free == NULL)
		return -1;
	fd = fopen("/proc/meminfo", "r");
	if (fd == NULL)
	{
		perror("/proc/meminfo");
		return -1;
	}
	while (fgets(line, sizeof(line), fd))
	{
		sscanf(line, "%[^:] %*[^ ]  %ld %*[^\n ]", name, &ma);
		if (strcmp(name, "MemFree") == 0)
		{
			*free = ma * 1024;
			break;
		}

	}
	fclose(fd);
	return 0;
}
int memAlloc()
{
	long total, free;
	long alloc;
	char *buf;
	get_system_memory_size(&total);
	get_system_memory_free(&free);
	alloc = free - total * 0.05;
	buf = (char *) malloc(alloc);
//	printf("total:%d,free:%d,alloc:%d\n",total,free,alloc);
	if (buf != NULL)
		memset(buf, 0xa5, alloc);
	return 0;
}

int main(int argc, char *argv[])
{
	pthread_t handle;
	int fd;
	fd = open("/dev/tty1", O_RDWR);
	write(fd, "\033[9;0]", 8);
	close(fd);
	fd = open("/dev/tty2", O_RDWR);
	write(fd, "\033[9;0]", 8);
	close(fd);

	pthread_create(&handle, NULL, comTestThread, NULL);

//	comTest();
	sleep(10);
	memAlloc();
	while (1)
	{
		;
	}
	return 0;
}

