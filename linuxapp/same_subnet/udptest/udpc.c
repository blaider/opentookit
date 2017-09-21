/*
 * File:   udpc.c
 * UDP client
 *
 * 主要实现：每秒发送一个文本消息
 */

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

int main(int argc, char **argv)
{
	char * ifname = NULL;
	/* 服务端地址 */
	struct sockaddr_in server_addr;

	if (argc < 3)
	{
		printf("useage:./udpc ipaddress port [interface]\n ");
		printf("\teg:./udpc  127.0.0.1 5555 eth0\n");
		return -1;
	}

	int destport = atoi(argv[2]);

	if(argc >= 4)
	{
		ifname = argv[3];
	}

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(destport);

	/* 创建socket */
	int client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (client_socket_fd < 0)
	{
		perror("Create Socket Failed:");
		exit(1);
	}

	if(ifname != NULL)
	{
		if ( setsockopt(
				client_socket_fd,
			SOL_SOCKET,
			SO_BINDTODEVICE,
			ifname,
			strlen( ifname ) ) )
		{
			return 0;
		}
	}

	int index=0;
	char buffer[BUFFER_SIZE];
	bzero(buffer, BUFFER_SIZE);

	while (1)
	{
		sprintf(buffer, "%s %d", "udp send data",index++);
		int n_send_len;
		n_send_len = sendto(client_socket_fd, buffer, strlen(buffer), 0,
				(struct sockaddr*) &server_addr, sizeof(server_addr));

		if(n_send_len < 0)
		{
			perror("socket send");
			break;
		}
		printf("send:[%d]%s\n", n_send_len, buffer);

		struct sockaddr_in client_addr;
		size_t client_addr_length = 0;
		int nread = recvfrom(client_socket_fd, buffer, BUFFER_SIZE, 0,
						(struct sockaddr*) &client_addr, &client_addr_length);
		if ( nread == -1)
		{
			perror("Receive Data Failed:");
			exit(1);
		}
		printf("recv:[%d]%s\n", nread, buffer);
		sleep(1);
	}

	close(client_socket_fd);
	return 0;
}
