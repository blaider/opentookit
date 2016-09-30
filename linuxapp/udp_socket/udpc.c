/*
 * File:   udpc.c
 * UDP client
 *
 * 主要实现：每秒发送一个文本消息
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

//int port=6789;
int main(int argc, char** argv) {
    int socket_descriptor;
    int iter=0;
    char buf[80];
    struct sockaddr_in address;
    int ret;
    struct addrinfo hints;
	struct addrinfo *res, *cur;
	int find = 0;

	 bzero(&address,sizeof(address));
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET; /* Allow IPv4 */
	hints.ai_flags = 0;// AI_PASSIVE;/* For wildcard IP address */
	hints.ai_protocol = 0; /* Any protocol */
	hints.ai_socktype = SOCK_DGRAM;

	ret = getaddrinfo("127.0.0.1", "6789", &hints, &res);

	if (ret == -1)
	{
		perror("getaddrinfo");
		exit(1);
	}
	for (cur = res; cur != NULL; cur = cur->ai_next)
	{
		memcpy(&address,cur->ai_addr,sizeof(struct sockaddr_in));
		find++;
		break;
	}
	freeaddrinfo(res);

	if(!find)
		return -1;
    //创建一个 UDP socket

    socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);

    do
    {
         /*
         * sprintf(s, "%8d%8d", 123, 4567); //产生：" 123 4567"
         * 将格式化后到 字符串存放到s当中
         */
        sprintf(buf,"data packet with ID %d\n",iter++);

        sendto(socket_descriptor,buf,sizeof(buf),0,(struct sockaddr *)&address,sizeof(address));
        sleep(1);
    }while(1);

    sprintf(buf,"stop\n");
    sendto(socket_descriptor,buf,sizeof(buf),0,(struct sockaddr *)&address,sizeof(address));//发送stop 命令
    close(socket_descriptor);
    printf("Messages Sent,terminating\n");

    exit(0);

    return (EXIT_SUCCESS);
}
