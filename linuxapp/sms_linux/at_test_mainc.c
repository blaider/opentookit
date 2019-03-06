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

char SERIALPORT[64] = {"/dev/ttyUSB3"};
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
	strcpy(buff, "AT+CSQ\r");
	nwrite = write(fd, buff, strlen(buff));
	if(7 != nwrite )
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
	int rssi=0,equ=0;
	char *pos=strchr(reply,':');
	if(NULL == pos)
	{
		close(fd);
		return -1;
	}
	sscanf(pos+2,"%d,%d",&rssi,&equ);
	printf("rssi:%d,equ:%d\n",rssi,equ);
	if(rssi >31 || rssi < 1)
	{
		close(fd);
		return 0;
	}
	*signal=(double)((int)(100.0*rssi/31));
	close(fd);
	return 0;
}

int main(int argc,char *argv[])
{
	if(argc > 1)
	{
		strcpy(SERIALPORT,argv[1]);
	}
	printf("serial port:%s\n",SERIALPORT);
	double signal;
	int i = 0;
	while(1)
	{
		test(&signal);
		printf("%4d:%lf\n",i++,signal);
	}
	return 0;

}
