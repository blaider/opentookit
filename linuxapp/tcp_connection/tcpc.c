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
#include <pthread.h>

struct sockaddr_in dest_addr;

void * thread_run(void *data)
{
	int index = (int)data;
	char buf[1024];
	int sockfd ;
	pthread_detach(pthread_self());
	if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0)))
		{
			perror("error in create socket\n");
			exit(0);
		}

	//connect
		if (-1 == connect(sockfd, (struct sockaddr*) &dest_addr,
						sizeof(struct sockaddr)))
		{
			perror("connect error\n");
			exit(0);
		}

		while (1)
		{
			sprintf(buf, "%s %d", "tcp send data",index);
			int n_send_len;
			n_send_len = send(sockfd, buf, strlen(buf), MSG_NOSIGNAL);

			if(n_send_len < 0)
			{
				perror("socket send");
				break;
			}
			printf("send:[%d]%s\n", n_send_len, buf);
			break;
		}
//		shutdown(sockfd,0);
//		close(sockfd);
		pthread_exit(0);
		return NULL;
}

int main(int argc, char **argv)
{
	int index = 100;
	pthread_t thid;

	if (argc < 3)
	{
		printf("useage:./tcpc ipaddress port [interface]\n ");
		printf("\teg:./tcpc  127.0.0.1 5555 eth0\n");
		return -1;
	}
	int destport = atoi(argv[2]);

	if(argc >= 4)
		index = atoi(argv[3]);

	memset(&dest_addr,0,sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(destport);
	dest_addr.sin_addr.s_addr = inet_addr(argv[1]);

	while(index--)
	{
		pthread_create(&thid,NULL,thread_run,(void *)index);
		usleep(200*100);
	}
	while(1)
	sleep(index/10+5);
	return 0;
}
