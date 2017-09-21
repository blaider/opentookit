/*************************************************************************
 > File Name: server.c
 ************************************************************************/
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<netdb.h>
#include<stdarg.h>
#include<string.h>

#define SERVER_PORT 5555
#define BUFFER_SIZE 1024

static void showUsage()
{
	printf("udps [port] [interface]\n");
	printf("etc. udps 5555 eth0\n");
}

int main(int argc, char **argv)
{
	char * ifname = NULL;
	/* 创建UDP套接口 */
	struct sockaddr_in server_addr;
	int serverport;

	showUsage();

	if (argc >= 2)
	{
		serverport = atoi(argv[1]);
	}
	else
	{
		serverport = SERVER_PORT;
	}

	if(argc >= 3)
	{
		ifname = argv[2];
	}

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(serverport);

	/* 创建socket */
	int server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_socket_fd == -1)
	{
		perror("Create Socket Failed:");
		exit(1);
	}

	if(ifname != NULL)
	{
		if ( setsockopt(
				server_socket_fd,
			SOL_SOCKET,
			SO_BINDTODEVICE,
			ifname,
			strlen( ifname ) ) )
		{
			return 0;
		}
	}

	/* 绑定套接口 */
	if (-1 == (bind(server_socket_fd, (struct sockaddr*) &server_addr,
					sizeof(server_addr))))
	{
		perror("Server Bind Failed:");
		exit(1);
	}
	printf("bind port %u success\n",ntohs(server_addr.sin_port = htons(serverport)));

	char buffer[BUFFER_SIZE];
	/* 数据传输 */
	while (1)
	{
		/* 定义一个地址，用于捕获客户端地址 */
		struct sockaddr_in client_addr;
		socklen_t client_addr_length = sizeof(client_addr);

		/* 接收数据 */
		bzero(buffer, BUFFER_SIZE);
		int nread = recvfrom(server_socket_fd, buffer, BUFFER_SIZE, 0,
				(struct sockaddr*) &client_addr, &client_addr_length);
		if ( nread == -1)
		{
			perror("Receive Data Failed:");
			exit(1);
		}
		printf("from ip:%s port:%d,[%d]%s\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port),nread,buffer);
		if (sendto(server_socket_fd, buffer, strlen(buffer), 0,
					(struct sockaddr*) &client_addr, sizeof(client_addr)) < 0)
		{
			perror("Send Failed:");
			exit(1);
		}
	}
	close(server_socket_fd);
	return 0;
}
