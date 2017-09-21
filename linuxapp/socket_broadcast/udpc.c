#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


int main(int argc ,char *argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);
    fflush(stdout);

    //从广播地址发送消息
   char smsg[2000] = {"abcdef"};
   memset(smsg,'a',sizeof(smsg));
    int sendcount = sizeof(smsg);
    int sock = -1;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
    	perror("socket");
        return 0;
    }

    const int opt = 1;
    //设置该套接字为广播类型，
    int nb = 0;
    nb = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
    if(nb == -1)
    {
    	perror("setsockopt");
        return 0;
    }

    struct sockaddr_in addrto;
    bzero(&addrto, sizeof(struct sockaddr_in));
    addrto.sin_family=AF_INET;
    addrto.sin_addr.s_addr=htonl(INADDR_BROADCAST);
    addrto.sin_port=htons(6513);
    int nlen=sizeof(addrto);

    if(argc >=2 )
    	sendcount = atoi(argv[1]);

    if(sendcount > sizeof(smsg))
		sendcount = sizeof(smsg);
    int test = 0;

    if(sendcount == 0)
    	test = 1;

    while(1)
    {
        usleep(10*1000);
        if(test)
        	sendcount++;

        int ret=sendto(sock, smsg, sendcount, 0, (struct sockaddr*)&addrto, nlen);
        if(ret<0)
        {
           printf("send error\n");
        }
        else
        {
            printf("send [%d]bytes ok\n",ret);
        }
    }

    return 0;
}
