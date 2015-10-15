/*************************************************************************
	> File Name: csq_mainc.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Fri 13 Mar 2015 02:09:10 PM CST
 ************************************************************************/


#include<stdio.h>

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

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

void swap(char number[], char swap[])
{
	char ch1[] = "86";
	char tmp[16];
	int i;

	memset(swap, 0, 32);
	memset(tmp, 0, 16);
	strcpy(swap, number);
	strcat(swap, "f");
	strcat(ch1, swap);
	strcpy(swap, ch1);

	for (i = 0; i <= strlen(swap) - 1; i += 2)
	{
		tmp[i + 1] = swap[i];
		tmp[i] = swap[i + 1];
	}
	strcpy(swap, tmp);
}
void inversSwap(char number[], char swap[],int len)
{
	int i;

	memset(swap, 0, len+1);

	for (i = 0; i <= len - 1; i += 2)
	{
		swap[i + 1] = number[i];
		swap[i] = number[i + 1];
	}
}
parsePDU(char *pdu)
{
	char buf[128];
	char *p=pdu;
	printf("SMSC LEN:%c%c\n",*p++,*p++);
	printf("TYPE:%c%c\n",*p++,*p++);
	inversSwap(p,buf,14);
	p+=14;
	printf("Center phone:%s\n",buf);
	printf("left:%c%c\n",*p++,*p++);
	printf("SMSC LEN:%c%c\n",*p++,*p++);
	printf("TYPE:%c%c\n",*p++,*p++);
	inversSwap(p,buf,14);
	p+=14;
	printf("src phone:%s\n",buf);
	printf("left:%s\n",p);

}

#define SERIALPORT "/dev/ttyUSB3"
int test(d)
{
	int fd;
	int nread, nwrite;
	char buff[128];
	char reply[2048];


	fd = open(SERIALPORT, O_RDWR | O_NOCTTY | O_NDELAY);
	if (-1 == fd)
	{
		perror(SERIALPORT);
		return -1;
	}
	serial_init(fd);

	//at
	memset(buff, 0, sizeof(buff));
	strcpy(buff, "at\r");
	nwrite = write(fd, buff, strlen(buff));
	printf("nwrite=%d,%s\n", nwrite, buff);
	if(0 > nwrite)
		return -1;

	memset(reply, 0, sizeof(reply));
	sleep(1);
	nread = read(fd, reply, sizeof(reply));
	printf("nread=%d,%s\n", nread, reply);
	if(0 > nread)
		return -1;

	//at+cmgr
	memset(buff, 0, sizeof(buff));
	strcpy(buff, "AT+CMGR=2\r");
	nwrite = write(fd, buff, strlen(buff));
	printf("nwrite=%d,%s\n", nwrite, buff);
	if(0 > nwrite)
		return -1;

	memset(reply, 0, sizeof(reply));
	sleep(1);
	nread = read(fd, reply, sizeof(reply));
	printf("nread=%d,%s\n", nread, reply);
	if(0 > nread)
		return -1;
	if(NULL == strstr(reply,"OK"))
	{
		close(fd);
		return -1;
	}
	int stat=0,len=0;
	char ala[1024];
	char *pos=strchr(reply,':');
	if(NULL == pos)
	{
		close(fd);
		return -1;
	}
	sscanf(pos+2,"%d,,%d\n%s",&stat,&len,ala);
	printf("stat:%d,len:%d,%s\n",stat,len,ala);
	parsePDU(ala);
	close(fd);
	return 0;
}

int main()
{
	test();
	return 0;

}
