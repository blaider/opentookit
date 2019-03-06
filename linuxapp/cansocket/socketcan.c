/*************************************************************************
 > File Name: socketcan.c
 > Author: suchao.wang
 > Mail: suchao.wang@advantech.com.cn
 > Created Time: Mon 04 Mar 2019 05:20:07 PM CST
 ************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <string.h>

/* At time of writing, these constants are not defined in the headers */
#ifndef PF_CAN
#define PF_CAN 29
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif

/* … */

/* Somewhere in your app */
int main()
{
	daemon(0,0);
	/* Create the socket */
	int skt = socket( PF_CAN, SOCK_RAW, CAN_RAW);

	/* Locate the interface you wish to use */
	struct ifreq ifr;
	strcpy(ifr.ifr_name, "can0");
	ioctl(skt, SIOCGIFINDEX, &ifr); // ifr.ifr_ifindex gets filled
			/* with that device’s index */

	/* Select that CAN interface, and bind the socket to it. */
	struct sockaddr_can addr;
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	bind(skt, (struct sockaddr *) &addr, sizeof(addr));

	/* Send a message to the CAN bus */
	struct can_frame frame;
	frame.can_id = 0x123 | CAN_EFF_FLAG;
	//strcpy(frame.data, "foo");
	frame.can_dlc = 8;// strlen(frame.data);

	int i = 0;

	while(1)
	{
		for(i=0;i<256;i++)
		{
			frame.can_id = i | CAN_EFF_FLAG;
			memset(frame.data,i,sizeof(frame.data));
			int bytes_sent = write(skt, &frame, sizeof(frame));
			usleep(1000*100);
		}
	}
	//int bytes_sent = write(skt, &frame, sizeof(frame));

	/* Read a message back from the CAN bus */
	//int bytes_read = read(skt, &frame, sizeof(frame));
	close(skt);
}
