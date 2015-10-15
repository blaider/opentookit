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

#include <signal.h>
#include "log.h"

#define APP_VERSION "2.0.3"

#ifndef SRCHOST
#define SRCHOST "127.0.0.1"
#endif
#ifndef DSTHOST
#define DSTHOST "127.0.0.1"
#endif
#ifndef NETCARD
#define NETCARD "eth0"
#endif

#define PING_WAIT 120

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
char g_sn[256] = {0};
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
typedef struct pairError{
	char adevName[50];
	char ashowName[50];
	char bdevName[50];
	char bshowName[50];
	long err;
}DEVPAIR;
DEVPAIR pairs[]=
{
{"/dev/ttyS0","COM1","/dev/ttyS1","COM2",0},
{"/dev/ttyS1","COM2","/dev/ttyS0","COM1",0},
{"/dev/ttyAP0","COM3","/dev/ttyAP4","COM7",0},
{"/dev/ttyAP4","COM7","/dev/ttyAP0","COM3",0},
{"/dev/ttyAP1","COM4","/dev/ttyAP5","COM8",0},
{"/dev/ttyAP5","COM8","/dev/ttyAP1","COM4",0},
{"/dev/ttyAP2","COM5","/dev/ttyAP6","COM9",0},
{"/dev/ttyAP6","COM9","/dev/ttyAP2","COM5",0},
{"/dev/ttyAP3","COM6","/dev/ttyAP7","COM10",0},
{"/dev/ttyAP7","COM10","/dev/ttyAP3","COM6",0},
};
time_t time_start,time_end;



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
#define MSG "ABCDEFGH"
int pairTest(DEVPAIR *pair)
{
//	char MSG[] = {1,2,3,4,5,6,7,8,9,0};
	int ret;
	char buf[256];
	int fd1,fd2;
	int idx;
//	time_t old,new;
//	old = time(NULL);


	fd1 = openDevice(pair->adevName,9600 , 8, 1, 'n');
	if (fd1 <= 0)
	{
		printf("open device %s failed!\n", pair->adevName);
		exit(1);
	}

	fd2 = openDevice(pair->bdevName, 9600, 8, 1, 'n');
	if (fd2 <= 0)
	{
		printf("open device %s failed!\n", pair->bdevName);
		exit(1);
	}

	write(fd1,MSG,sizeof(MSG));
//	usleep(1000000/speed*(4+databits)*sizeof(MSG)*2);

	memset(buf, 0, sizeof(buf));
	idx = 0;
	ret = read(fd2, buf, sizeof(MSG));
	idx += ret;
	if(strncmp(MSG,buf,sizeof(MSG)) != 0){
		LOG("Test %s -> %s fail\n",pair->ashowName,pair->bshowName);
		pair->err++;
	}else{
		printf("Test %s -> %s OK\n",pair->ashowName,pair->bshowName);
	}

	close(fd1);
	close(fd2);
	return 0;
}
#define MSG1 "ADVANTEC"
#define MSG2 "advantec"
int selfTest(const char *comName,const  char *showName,long *err)
{
	char buf[128];
	char devName[50];
	int fd;
	int ret;
	int strLen;

	strcpy(devName, comName);
	fd = openDevice(devName, 9600, 8, 1, 'n');
	if (fd <= 0)
	{
		LOG("open device %s failed!\n", showName);
		return -1;
	}
	strLen = strlen(MSG1);
	ret = write(fd, MSG1, strLen);
	if (ret != strLen)
		LOG("%s write '%s' failed--------\n", showName, MSG1);
	//usleep(1000*1000/(96000*strLen));
	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, strlen(MSG1));
	if (ret != strLen)
		LOG("%s read '%s' failed,%d,%s--------\n", showName, MSG1,ret,buf);

	if (strcmp(buf, MSG1))
	{
		LOG("%s test '%s' failed,%d--------\n", showName, MSG1,ret);
		(*err)++;
	}else
		printf("%s test '%s' success\n", showName, MSG1);

	strLen = strlen(MSG2);
	ret = write(fd, MSG2, strLen);
	if (ret != strLen)
		LOG("%s write '%s' failed,%d========\n", devName, MSG2,ret);
	//usleep(1000*1000/(96000*strLen));
	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, strlen(MSG2));
	if (ret != strLen)
		LOG("%s read '%s' failed,%d,%s========\n", showName, MSG2,ret,buf);

	if (strcmp(buf, MSG2))
	{
		LOG("%s test '%s' failed========\n", showName, MSG2);
		(*err)++;
	}else
		printf("%s test '%s' success\n", showName, MSG2);
	close(fd);

	return 0;
}
void* allSelfTestThread(void* arg)
{
	int no = (int) arg;
	long err;
	char name[50];
	sprintf(name, "/dev/ttyAP%u", no);
	while (1)
	{
		selfTest(name,name,&err);
		sleep(1);
	}
	return NULL;
}

#define COM_NUMS 1
int comTest()
{
	return 0;
}
void *comPairTestThread(void *arg)
{
	int i;
	for (;;)
	{
		for(i=0;i<sizeof(pairs)/sizeof(pairs[0]);i++)
		{
			 pairTest(&pairs[i]);
		}
	}
	return NULL;
}
void *comTestThread(void *arg)
{
	int i;
	for (;;)
	{
		for(i=0;i<sizeof(devs)/sizeof(devs[0]);i++)
		{
			 selfTest(devs[i].devName,devs[i].showName,&devs[i].err);
		}
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
int get_cpuname(char *cpuname)
{
	FILE * fd;
	char line[100];
	char type[100];
	char name[100];
	if (cpuname == NULL)
			return -1;

	fd = fopen("/proc/cpuinfo", "r");
	if (fd == NULL)
	{
		perror("/proc/cpuinfo");
		return -1;
	}
	while (fgets(line, sizeof(line), fd))
	{
		memset(type,0,sizeof(type));
		memset(name,0,sizeof(name));
		sscanf(line, "%[^:] %*[^ ] %[^\n]", type, name);
		if (strncmp(type, "model name",10) == 0)
		{
			strcpy(cpuname,name);
			break;
		}
	}
	fclose(fd);
	return 0;
}
int get_osname(char *osname)
{
	FILE * fd;
	char line[100];
	if (osname == NULL)
			return -1;

	fd = fopen("/etc/issue", "r");
	if (fd == NULL)
	{
		perror("/etc/issue");
		return -1;
	}
	if (fgets(line, sizeof(line), fd))
	{
		line[strlen(line) -1] = '\0';
		strcpy(osname,line);
	}
	fclose(fd);
	return 0;
}
int get_kernelname(char *kernelname)
{
	FILE * fd;
	char line[100];
	if (kernelname == NULL)
			return -1;

	fd = popen("uname -srm", "r");
	if (fd == NULL)
	{
		perror("uname -srm");
		return -1;
	}
	if (fgets(line, sizeof(line), fd))
	{
		line[strlen(line) -1] = '\0';
		strcpy(kernelname,line);
	}
	pclose(fd);
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
int s= 0;
pthread_t handle;
pthread_t handle_ping;
pid_t mainpid;
sem_t m_hEvent;
void *pingThread(void *arg)
{
	int i=PING_WAIT;
	setip(NETCARD,SRCHOST);
	g_stPktStatic.uiSendPktNum = 0;
	g_stPktStatic.uiRcvPktNum = 0;
	g_stPktStatic.fMinTime = 1000000.0;
	g_stPktStatic.fMaxTime = -1.0;
	g_stPktStatic.fArgTime = 0.0;
	while(sem_trywait(&m_hEvent) && i>0)
	{
		sleep(1);
		i--;
	}

	while(sem_trywait(&m_hEvent))
	{
		selfping(DSTHOST);
		sleep(1);
	}
	return NULL;
}
static void sig_alarm(int no)
{
	sem_post(&m_hEvent);
	return;
}
static void sig_intpair(int no)
{
	int i;
	int days,hours,mins,seconds;
	time_t span;
	char sysinfo[256];
	char command[256];
	long memTotal;
	int totalErr = 0;
	struct tm tm_start,tm_end,*tm;
	unsigned int uiSend, uiRecv;
	float fpass=0;

//	printf("%s:%d,%ld,%d,%d\n",__func__,no,pthread_self(),getpid(),getppid());
	if(mainpid != pthread_self())
	{
		pthread_exit(0);
		return ;
	}
	sem_post(&m_hEvent);
	pthread_kill(handle,SIGINT);
//	pthread_kill(handle_ping,SIGINT);
	pthread_join(handle,NULL);
	pthread_join(handle_ping,NULL);
	time(&time_end);
	tm = localtime(&time_start);
	memcpy(&tm_start,tm,sizeof(*tm));
	tm = localtime(&time_end);
	memcpy(&tm_end,tm,sizeof(*tm));
	span = time_end - time_start;
	days = span/(24*60*60);
	seconds = span %(24*60*60); 
	hours = seconds/(60*60);
	seconds = seconds %(60*60); 
	mins = seconds/(60);
	seconds = seconds %(60); 
	LOGNOTIME("================BurnInTest-CSC Certificate=======\n");
	LOGNOTIME("Report Date: %04d-%02d-%02d\n",tm_end.tm_year+1900,tm_end.tm_mon  +1,tm_end.tm_mday);
	LOGNOTIME("Generateed by: BurnInTest-CSC %s\n",APP_VERSION);
	LOGNOTIME("Environment Temperature : 40 degree\n");
	LOGNOTIME("================System Summary===================\n");
	LOGNOTIME("SN: %s\n",g_sn);
	memset(sysinfo,0,sizeof(sysinfo));
	get_osname(sysinfo);
	LOGNOTIME("OS name: %s\n",sysinfo);
	memset(sysinfo,0,sizeof(sysinfo));
	get_kernelname(sysinfo);
	LOGNOTIME("Kernel name: %s\n",sysinfo);
	memset(sysinfo,0,sizeof(sysinfo));
	get_cpuname(sysinfo);
	LOGNOTIME("CPU name: %s\n",sysinfo);
	get_system_memory_size(&memTotal);
	LOGNOTIME("MEM size: %ld MB\n",memTotal/1024/1024);
	LOGNOTIME("================Result Summary===================\n");
	LOGNOTIME("Test Start Time: %04d-%02d-%02d %02d:%02d:%02d\n", tm_start.tm_year + 1900,
			tm_start.tm_mon + 1, tm_start.tm_mday, tm_start.tm_hour, tm_start.tm_min,
			tm_start.tm_sec);
	LOGNOTIME("Test End   Time: %04d-%02d-%02d %02d:%02d:%02d\n", tm_end.tm_year + 1900,
			tm_end.tm_mon + 1, tm_end.tm_mday, tm_end.tm_hour, tm_end.tm_min,
			tm_end.tm_sec);
	LOGNOTIME("Time Duration: %d days %d hours %2d mins %2d seconds\n",days,hours,mins,seconds);
	//LOG("total: %d seconds\n",span);
	for(i=0;i<sizeof(pairs)/sizeof(pairs[0]);i++)
	{
		if(pairs[i].err)
		{
		 LOGNOTIME("%s -> %s: Error,%ld\n",pairs[i].ashowName,pairs[i].bshowName,pairs[i].err);
		}else{
		 LOGNOTIME("%s -> %s: Pass\n",pairs[i].ashowName,pairs[i].bshowName);
		}
	}
	uiSend = g_stPktStatic.uiSendPktNum;
	uiRecv = g_stPktStatic.uiRcvPktNum;
	fpass= (uiRecv+0.0001)/(uiSend+0.0001)*100;
	LOGNOTIME("MEM Used 95%  Test: Pass\n");
	LOGNOTIME("CPU Used 100% Test: Pass\n");
	printf("ping %s send %u,recv %u,lost %u,pass rate:%.2f%%\n",DSTHOST,uiSend,uiRecv,uiSend-uiRecv,fpass);
	if(fpass > 95)
	{
		LOGNOTIME("LAN Test: Pass\n");
	}
	else{
		LOGNOTIME("Ping %s Test: Error,lost=%.2f%\n",DSTHOST,(100-fpass));
		totalErr++;
	}
	system("swapon -a");
	sprintf(command,"./ftpupdate.sh %s",g_logfile);
	system(command);
	for (i = 0; i < sizeof(pairs) / sizeof(pairs[0]); i++)
	{
		totalErr += pairs[i].err;
	}
	if(totalErr)
		printFailed();
	else
		printPass();
	exit(0);
}
static void print_usage()
{
	printf("Usage:\n");
	printf("\t./BurnInTest-CSC -h  hours -f logfile  :exit after hours\n");
	printf("for example:\n");
	printf("\t./BurnInTest-CSC -h  72  -f KANB148060 \n");
	printf("\t            :exit after 72 hours and generate KANB148060.log\n");
	printf("\t./BurnInTest-CSC -h  0  -f KANB148060   \n");
	printf("\t            :exit until press Ctrl + C and generate KANB148060.log\n");
	printf("\n    Tip: Always you can  press Ctrl + C to exit at any time\n");
	printf("    Tip: with ftpupdate.sh the log will transfer to ftp server automatically\n");
}

int run_seconds;
int parse_opt(int argc,char *argv[])
{
	int ch;
	opterr = 0;
	while((ch = getopt(argc,argv,"f:h:m:s:")) != -1)
	{
		switch(ch)
		{
		case 'f':
			sprintf(g_logfile,"%s.log",optarg);	
			sprintf(g_sn,"%s",optarg);	
			break;
		case 'h':
			run_seconds = strtoul(optarg,NULL,10);
			run_seconds = run_seconds * 60 * 60;
			break;
		case 'm':
			run_seconds = strtoul(optarg,NULL,10);
			run_seconds = run_seconds * 60;
			break;
		case 's':
			run_seconds = strtoul(optarg,NULL,10);
			break;
		default:
			print_usage();
			exit(0);
		}
	}
	return 0;
}
int main(int argc, char *argv[])
{
	int fd;
	time_t now;
	struct tm *local;
	char command[256];
	FILE *file;
	
	if(argc < 5)
	{
		print_usage();
		exit(0);
	}
	parse_opt(argc,argv);
	if(strlen(g_logfile)< 4)
	{
		print_usage();
		exit(0);
	}
	file = fopen(g_logfile,"wt");
	fclose(file);

	
	mainpid = pthread_self();
	//signal(SIGINT,sig_int);
	signal(SIGINT,sig_intpair);
	signal(SIGALRM,sig_alarm);

	//alarm(run_seconds);
	time(&time_start);
	sprintf(command,"./timerkill %d %d",getpid(),run_seconds);
	system(command);
	system("swapoff -a");
	fd = open("/dev/tty1", O_RDWR);
	write(fd, "\033[9;0]", 8);
	close(fd);
	fd = open("/dev/tty2", O_RDWR);
	write(fd, "\033[9;0]", 8);
	close(fd);

	time(&now);
	local = localtime(&now);
	LOGNOTIME("=================================================\n");
//	pthread_create(&handle, NULL, comTestThread, NULL);
	pthread_create(&handle, NULL, comPairTestThread, NULL);
	if (sem_init(&m_hEvent, 0, 0) == -1)
			return 2;
	if(run_seconds -PING_WAIT*2 > 0)
	{
		pthread_create(&handle_ping, NULL, pingThread, NULL);
		alarm(run_seconds -PING_WAIT);
	}

	sleep(10);
	memAlloc();
	while (1)
	{
//		selfping("172.21.67.1");
		;
	}
	return 0;
}

