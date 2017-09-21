/*************************************************************************
	> File Name: multiserver.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 16 Aug 2017 03:55:07 PM CST
 ************************************************************************/
/* 接收端 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#define MSGBUFSIZE 256
#define TRAM_STATUS_ADDR "225.0.0.3"
#define TRAM_STATUS_RECV_PORT 8002

int main()
{
    struct sockaddr_in addr;
    int fd, nbytes, addrlen;
    struct ip_mreq mreq;
    char msgbuf[MSGBUFSIZE];
    int on;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket ");
        return -1;
    }
    /* 初始化地址 */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(TRAM_STATUS_RECV_PORT);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return -1;
    }

    on = 1;
    /* 设置地址复用许可, 根据具体情况判断是否增加此功能 */
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        perror("SO_REUSEADDR");
        return -1;
    }

    /*加入多播组*/
    mreq.imr_multiaddr.s_addr = inet_addr(TRAM_STATUS_ADDR);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);//inet_addr("172.21.67.141");
    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        perror("setsockopt");
        return -1;
    }

    while(1)
    {
		addrlen = sizeof(addr);
		if ((nbytes = recvfrom(fd, msgbuf, MSGBUFSIZE, 0, (struct sockaddr *)&addr, (socklen_t *)&addrlen)) < 0)
		{
			perror("recvfrom");
			return -1;
		}
		struct in_addr addr1;
		memcpy(&addr1, &addr.sin_addr.s_addr, 4);
		printf("recv ok[%s] from %s!\n",msgbuf,inet_ntoa(addr1));
    }

    /*退出多播组*/
    setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
    close(fd);

    return 0;
}


