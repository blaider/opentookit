/*************************************************************************
 > File Name: tcpc.c
 ************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
	int index = 0;
	char buf[1024];
	int sockfd;
	struct sockaddr_in dest_addr;
	if (argc != 3)
	{
		printf("useage:./tcpc ipaddress port\n ");
		printf("\teg:./tcpc  127.0.0.1 5555\n");
		return -1;
	}
	int destport = atoi(argv[2]);
	if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0)))
	{
		perror("error in create socket\n");
		exit(0);
	}
	memset(&dest_addr,0,sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(destport);
	dest_addr.sin_addr.s_addr = inet_addr(argv[1]);
	//connect
	if (-1 == connect(sockfd, (struct sockaddr*) &dest_addr,
					sizeof(struct sockaddr)))
	{
		perror("connect error\n");
		exit(0);
	}


	while (1)
	{
		sprintf(buf, "%s %d", "tcp send data",index++);
		int n_send_len;
		n_send_len = send(sockfd, buf, strlen(buf), MSG_NOSIGNAL);

		if(n_send_len < 0)
		{
			perror("socket send");
			break;
		}
		printf("send:[%d]%s\n", n_send_len, buf);

		int nread = recv(sockfd, buf, sizeof(buf), 0);
		if (nread > 0)
		{
			printf("receive:[%d]%s\n", nread, buf);
		}
		if (nread < 0)
		{
			break;
		}
		sleep(1);
	}
	printf("exit program\n");
	shutdown(sockfd, 0);
	close(sockfd);
	return 0;
}
