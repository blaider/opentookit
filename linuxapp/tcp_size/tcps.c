/*************************************************************************
 > File Name: tcps.c
 ************************************************************************/
#include <sys/socket.h>
#include <unistd.h>// for close function
#include <string.h> // for bzero function
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>


#define SERV_PORT 5555
#define BACKLOG 10000 //the counts of connect can keep in wait queen
#define MAXBUFSIZE 200

static void showUsage()
{
	printf("tcps [port] [interface]\n");
	printf("etc. tcps 5555 eth0\n");
}

void * thread_run(void *data)
{
	char buf[MAXBUFSIZE]; //receive buf
	int sockfd_client = (int)data;
	printf("%s\n",__func__);
	pthread_detach(pthread_self());
	while (1)
	{
		memset(buf,0,sizeof(buf));
		int recvbytes; //the number of bytes receive from socket
		recvbytes = recv(sockfd_client, buf, MAXBUFSIZE, 0);
		if (0 >= recvbytes)
		{
			perror("receive");
			break;
		}
		printf("%d bytes receive from connect:%s\n", recvbytes, buf);
		if(recvbytes > 0) {
			recvbytes = send(sockfd_client, buf, recvbytes, MSG_NOSIGNAL);
		}else{
			recvbytes = send(sockfd_client, "heartbeat", strlen("heartbeat"), MSG_NOSIGNAL);
		}
		if(recvbytes < 0)
		{
			perror("send");
			break;
		}
	}
	printf("socket exit\n");
	shutdown(sockfd_client,0);
	close(sockfd_client);
	pthread_exit(NULL);

	return NULL;
}

int main(int argc, char **argv)
{
	int sockfd, sockfd_client = 0;
	socklen_t sin_size = sizeof(struct sockaddr_in);
	struct sockaddr_in serv_addr, client_sockaddr; //server ip info
	int serverport;
	pthread_t thid;

	showUsage();
	if (argc == 2)
	{
		serverport = atoi(argv[1]);
	}
	else
	{
		serverport = SERV_PORT;
	}

	if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0)))
	{
		perror("error in create socket\n");
		exit(0);
	}

	//set the sockaddr_in struct
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(serverport); //server listening port
	serv_addr.sin_addr.s_addr = INADDR_ANY; //here is the specia in listening tcp connect
//bind , the ip and port information is aready in the sockaddr
	if (-1 == bind(sockfd, (struct sockaddr*) &serv_addr,
					sizeof(struct sockaddr)))
	{
		perror("bind error\n");
		exit(0);
	}
	printf("bind seccessful\n");

	if (-1 == listen(sockfd, BACKLOG))
	{
		perror("lisenning");
		exit(1);
	}
	printf("the server is listenning...\n");
	//accept
	while(1)
	{
		if (-1 == (sockfd_client = accept(sockfd,
						(struct sockaddr*) &client_sockaddr, &sin_size)))
		{
			perror("accept");
			exit(1);
		}
		printf("accept connect from ip:%s port:%d\n",inet_ntoa(client_sockaddr.sin_addr),ntohs(client_sockaddr.sin_port));

		if (0 != pthread_create(&thid,NULL,thread_run,(void *)sockfd_client))
		{
			perror("pthread create");
			exit(1);
		}
	}
	printf("eixt program\n");

	shutdown(sockfd,0);
	close(sockfd);
	return 0;
}
