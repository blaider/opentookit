/*************************************************************************
	> File Name: libserialinfo.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Tue 05 Sep 2017 03:24:31 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/resource.h>
#include <syslog.h>

int socketfd = -1;
#define SOCKETFILE "/tmp/socket_gprs_serial"
int initSocket()
{
	int sockfd;
	int len;
	struct sockaddr_un address;
	int result;

	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket");
		return -1;
	}
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, SOCKETFILE);
	len = sizeof(address);
	result = connect(sockfd, (struct sockaddr*) &address, len);
	if (result == -1)
	{
		printf("ensure the wdtd is up\n");
		perror("connect");
		close(sockfd);
		return -1;
	}
	socketfd = sockfd;
	return 0;
}

static int getcount(char *buf,int len)
{
	int cmdcount = 0;
	char *p = NULL;
	char *pstart = buf;
	do
	{
		p = strchr(pstart,'\r');
		if( p!= NULL && p-buf < len)
		{
			cmdcount++;
			pstart = p+1;
		}else if(pstart-buf < len)
		{
			cmdcount++;
		}
	}while(p != NULL && p-buf < len);
	return cmdcount;
}

static int writeCommand(char *buf,int len)
{

	if(socketfd == -1)
	{
		initSocket();
	}

	if(socketfd == -1)
		return 0;

	int ret=0;
	int maxfdp;
	fd_set fds;
	struct timeval timeout={5,0}; //select等待3秒，3秒轮询，要非阻塞就置0

	FD_ZERO(&fds); //每次循环都要清空集合，否则不能检测描述符变化
	FD_SET(socketfd,&fds); //添加描述符
	maxfdp=socketfd+1;
	timeout.tv_sec=3;
	switch (select(maxfdp, NULL, &fds, NULL, &timeout))
	//select使用
	{
	case -1:
		socketfd=-1;
		break; //select错误，退出程序
	case 0:
		printf("wirte timeout\n");
		break; //再次轮询
	default:
		if (FD_ISSET(socketfd, &fds)) //测试sock是否可读，即是否网络上有数据
		{
			if (send(socketfd, buf, len,MSG_NOSIGNAL) == -1)
			{
				perror("write");
				socketfd=-1;
				return 0;
			}
		} // end if break;
	} // end switch
	FD_ZERO(&fds); //每次循环都要清空集合，否则不能检测描述符变化
	FD_SET(socketfd,&fds); //添加描述符
	maxfdp=socketfd+1;
	timeout.tv_sec=3*getcount(buf,len);
	switch (select(maxfdp, &fds, NULL, NULL, &timeout))
	//select使用
	{
	case -1:
		socketfd=-1;
		break; //select错误，退出程序
	case 0:
		ret = -1;
		printf("read timeout\n");
		break; //再次轮询
	default:
		if (FD_ISSET(socketfd, &fds)) //测试sock是否可读，即是否网络上有数据
		{
			char buff[2000];
			memset(buff,0,sizeof(buff));
			if (recv(socketfd, buff, sizeof(buff),0) == -1)
			{
				perror("read");
				socketfd=-1;
				return 0;
			}
			printf("read:command:%s\n",buff);
		} // end if break;
	} // end switch

	return ret;
}

int main(int argc,char *argv[])
{
	int type = 0;
	initSocket();
	int i = 10;
	if(argc >= 2)
		type = atoi(argv[1]);
	char *en_sms="AT+CMGF=1\rAT+CMGS=\"18600122492\"\rhello\r\x1a";
	for(i=0;i<10;i++)
	{
		switch(type)
		{
		case 1:
			writeCommand("at+cops?\r",strlen("at+cops?\r"));
			break;
		case 2:
			writeCommand(en_sms,strlen(en_sms));
			exit(0);
			break;
		default:
			writeCommand("at+cimi\r",strlen("at+cimi\r"));
			break;
		}
		sleep(1);
	}
	shutdown(socketfd,0);
	close(socketfd);
}
