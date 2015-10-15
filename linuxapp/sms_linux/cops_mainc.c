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
#include <ctype.h>

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

#define SERIALPORT "/dev/ttyUSB3"
int test(double *signal)
{
	int fd;
	int nread, nwrite;
	char buff[128];
	char reply[128];

	if(NULL ==signal )
		return -1;
	*signal=0;


	fd = open(SERIALPORT, O_RDWR | O_NOCTTY | O_NDELAY);
	if (-1 == fd)
	{
		perror(SERIALPORT);
		return -1;
	}
	serial_init(fd);

	memset(buff, 0, sizeof(buff));
	strcpy(buff, "AT+COPS?\r");
	nwrite = write(fd, buff, strlen(buff));
	if(9 != nwrite )
	{
		close(fd);
		return -1;
	}

	memset(reply, 0, sizeof(reply));
	usleep(10*1000);
	nread = read(fd, reply, sizeof(reply));
	if(7 > nread)
	{
		close(fd);
		return -1;
	}

	if(NULL == strstr(reply,"OK"))
	{
		close(fd);
		return -1;
	}
	char *p = reply;
	while((*p++)!='"');
	while((*p++)!='"');

	p++;
	char *pnull = p+1;
	while((*++pnull)!='\n');
	*pnull = 0;
	printf("%s\n",p);
	*signal=atof(p);;
	close(fd);
	return 0;
}

int main()
{
	double signal;
	test(&signal);
	printf("%lf\n",signal);
	return 0;

}
