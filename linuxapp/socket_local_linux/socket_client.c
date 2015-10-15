/*************************************************************************
 > File Name: socket_client.c
 > Author: suchao.wang
 > Mail: suchao.wang@advantech.com.cn
 > Created Time: Mon 02 Mar 2015 03:46:07 PM CST
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define WDT_SETMODD 1
#define WDT_ENABLE	2
#define WDT_DISABLE 3
#define WDT_STROBE	4
typedef struct _command
{
	char cmd;
	char mode;
	char reserver[2];
	int	paramter;
}CMD;

int writeCommand(int sockfd,CMD cmd)
{
	int ret=0;
	int maxfdp;
	fd_set fds;
	struct timeval timeout={5,0}; //select等待3秒，3秒轮询，要非阻塞就置0

	FD_ZERO(&fds); //每次循环都要清空集合，否则不能检测描述符变化
	FD_SET(sockfd,&fds); //添加描述符
	maxfdp=sockfd+1;
	timeout.tv_sec=3;
	switch (select(maxfdp, NULL, &fds, NULL, &timeout))
	//select使用
	{
	case -1:
		exit(-1);
		break; //select错误，退出程序
	case 0:
		printf("wirte timeout\n");
		break; //再次轮询
	default:
		if (FD_ISSET(sockfd, &fds)) //测试sock是否可读，即是否网络上有数据
		{
			CMD command;
			command.cmd=cmd.cmd;
			command.mode=cmd.mode;
			command.paramter=cmd.paramter;
			if (write(sockfd, &command, sizeof(command)) == -1)
			{
				perror("write");
				exit(1);
			}
		} // end if break;
	} // end switch
	FD_ZERO(&fds); //每次循环都要清空集合，否则不能检测描述符变化
	FD_SET(sockfd,&fds); //添加描述符
	maxfdp=sockfd+1;
	timeout.tv_sec=3;
	switch (select(maxfdp, &fds, NULL, NULL, &timeout))
	//select使用
	{
	case -1:
		exit(-1);
		break; //select错误，退出程序
	case 0:
		ret = -8;
		printf("read timeout\n");
		break; //再次轮询
	default:
		if (FD_ISSET(sockfd, &fds)) //测试sock是否可读，即是否网络上有数据
		{
			CMD command;
			if (read(sockfd, &command, sizeof(command)) == -1)
			{
				perror("read");
//					exit(1);
			}
			printf("read:command:%d,%d\n",command.cmd,command.paramter);
			ret = command.paramter;
		} // end if break;
	} // end switch

	return ret;
}

int main()
{
	int sockfd;
	int len;
	struct sockaddr_un address;
	int result;
	int i, byte;
	char ch_recv, ch_send;


	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket");
		exit(1);
	}
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, "server_socket");
	len = sizeof(address);
	result = connect(sockfd, (struct sockaddr*) &address, len);
	if (result == -1)
	{
		printf("ensure the server is up\n");
		perror("connect");
		exit(1);
	}
	CMD cmd;
	cmd.cmd=WDT_ENABLE;
	cmd.paramter=5;
	writeCommand(sockfd,cmd);
	for(i=0;i<5;i++)
	{
		cmd.cmd=WDT_STROBE;
		int ret =writeCommand(sockfd,cmd);
		printf("ret:%d\n",ret);
		sleep(2);

	}
	close(sockfd);
	return 0;
}

