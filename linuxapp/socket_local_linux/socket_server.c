/*************************************************************************
 > File Name: socket_server.c
 > Author: suchao.wang
 > Mail: suchao.wang@advantech.com.cn
 > Created Time: Mon 02 Mar 2015 03:45:57 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

int main()
{
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_un server_address;
	struct sockaddr_un client_address;
	int i, byte;
	char ch_send, ch_rec;
	unlink("server_socket");
	server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server_sockfd == -1)
	{
		perror("socket");
		exit(1);
	}
	server_address.sun_family = AF_UNIX;
	strcpy(server_address.sun_path, "server_socket");
	server_len = sizeof(server_address);

	bind(server_sockfd, (struct sockaddr *) &server_address, server_len);
	listen(server_sockfd, 5);
	printf("server waiting for client connect\n");
	client_len = sizeof(client_address);
	client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_address,
			(socklen_t *) &client_len);
	printf("the server wait for client data\n");
	for (i = 0, ch_send = '1'; i < 5; i++, ch_send++)
	{
		if ((byte = read(client_sockfd, &ch_rec, 1)) == -1)
		{
			perror("read");
//			exit(1);
		}
		printf("the character receiver from client is %c\n", ch_rec);
		sleep(i);
		if (write(client_sockfd, &ch_send, 1) == -1)
		{
			perror("write");
//			exit(1);
		}

	}
	close(client_sockfd);
	unlink("server_socket");
	return 0;
}

