/*************************************************************************
	> File Name: advtest.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Mon 12 Mar 2018 11:37:28 AM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdint.h>

#define LOGEX printf
#define STAT_IDLE 0

char m_status = 0;

#define SOCKETFILE "/tmp/socket_advprogrammgr"
int connectSocket()
{
	LOGEX("%s:connect server\n",__func__);
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
		LOGEX("ensure the AdvProgramMgr is up\n");
		perror("connect");
		close(sockfd);
		return -1;
	}
	return sockfd;
}

#pragma pack(1)
struct SOCKET_CMD
{
	char status;
	uint16_t len;
};
typedef struct SOCKET_CMD SOCCMD;

#pragma pack()


char m_errMsg[2048];
int m_errMsgLen = 0;
int m_socketfd;

uint32_t SendCommand(char status,int test)
{
	int waittime = 30;
	char buff[2048];
	SOCCMD *pc;
	int count = 5;

	if(test)
		waittime = 3;

	while( m_socketfd < 0 && count)
	{
		m_socketfd = connectSocket();
		count--;
		sleep(1);
	}
	if( m_socketfd < 0 )
	{
		memset(m_errMsg,0,sizeof(m_errMsg));
		sprintf(m_errMsg,"Manager is down;");
		m_errMsgLen = strlen(m_errMsg);
		m_status = STAT_IDLE;
		return 0;
	}
	m_errMsgLen = 0;
	memset(m_errMsg,0,sizeof(m_errMsg));
	buff[0] = status;
	if(status != STAT_IDLE)
		m_status = status;
	int n = send(m_socketfd,buff,1,MSG_NOSIGNAL);
	if( n <= 0 )
	{
		m_socketfd = connectSocket();
		n = send(m_socketfd,buff,1,MSG_NOSIGNAL);
		if(n < 0)
		{
			close(m_socketfd);
			m_socketfd = -1;
			memset(m_errMsg,0,sizeof(m_errMsg));
			sprintf(m_errMsg,"Manager is down;");
			m_errMsgLen = strlen(m_errMsg);
			m_status = STAT_IDLE;
			return 0;
		}
	}else{
//		LOGEX("%s:send %d bytes,%d\n",__func__,n,status);
	}
	int maxfdp;
	fd_set fds;
	struct timeval timeout={5,0}; //select等待3秒，3秒轮询，要非阻塞就置0
	int nread = 0;

	FD_ZERO(&fds); //每次循环都要清空集合，否则不能检测描述符变化
	FD_SET(m_socketfd,&fds); //添加描述符
	maxfdp=m_socketfd+1;
	timeout.tv_sec=waittime;
	//read select
	switch (select(maxfdp, &fds,NULL,  NULL, &timeout))
	//select使用
	{
	case -1:
		{
				close(m_socketfd);
				m_socketfd = -1;
				memset(m_errMsg,0,sizeof(m_errMsg));
				sprintf(m_errMsg,"Manager is down;");
				m_errMsgLen = strlen(m_errMsg);
				m_status = STAT_IDLE;
				return 0;
			}
		break;
	case 0:
//		printf("read timeout\n");
//		{
//			close(m_socketfd);
//			m_socketfd = -1;
//			memset(m_errMsg,0,sizeof(m_errMsg));
//			sprintf(m_errMsg,"Manager connect timeout;");
//			m_errMsgLen = strlen(m_errMsg);
//			m_status = STAT_IDLE;
//			return 0;
//		}
		break; //再次轮询
	default:
		if (FD_ISSET(m_socketfd, &fds)) //测试sock是否可读，即是否网络上有数据
		{
			int count = 3;

			while(count && nread !=3)
			{
				n = recv(m_socketfd, buff+nread, 3-nread,0);
				if ( n <= 0)
				{
					perror("read");
					close(m_socketfd);
					m_socketfd=-1;
					break;
				}
				count--;
				nread+=n;
			}
			if(nread !=3 )
			{
				return 0;
			}
		} // end if break;
	} // end switch
	LOGEX("%s:read %d\n",__func__,nread);
	if(nread !=3)
		return 0;
	pc = (SOCCMD *)buff;
	if( !test )
		m_status = pc->status;
	if( pc->len > 0)
	{
		LOGEX("%s:cmdlen %d bytes,%d\n",__func__,pc->len + 3,pc->status);
		m_errMsgLen = pc->len;
		memset(m_errMsg,0,sizeof(m_errMsg));
		if (recv(m_socketfd, m_errMsg, pc->len,0) == -1)
		{
			perror("read");
			m_socketfd=-1;
		}
		LOGEX("%s:cmdlen %d bytes,%s\n",__func__,pc->len + 3,m_errMsg);
	}

	return pc->len + 3;
}


int main()
{
	m_socketfd = connectSocket();
	while(1)
	{
		SendCommand(STAT_IDLE,0);
		printf("m_status = %d\n",m_status);
		sleep(1);
	}

}
