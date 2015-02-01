/********************************************************
 * PING
 * : GCC-4.2.4
 * YSQ-NJUST,yushengqiangyu@163.com
 *
 *******************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "log.h"

#ifndef DSTHOST
#define DSTHOST ""
#endif

#define E_FAILD_FD 			-1
#define ICMP_DATA_LEN 		20 /*  */
#define ICMP_ECHO_MAX		10/* ECHO-REQUEST */
#define ICMP_REQUEST_TIMEOUT 2 /*  */

/* ICMP  */
static unsigned char aucSendBuf[1024 * 1024] ={ 0 };
static unsigned char aucRecvBuf[1024 * 1024] ={ 0 };

/*  */
//typedef struct tagIcmpStatic
//{
//	unsigned int uiSendPktNum;
//	unsigned int uiRcvPktNum;
//	float fMinTime;
//	float fMaxTime;
//	float fArgTime;
//} ICMP_STATIC_S;

/*  */
ICMP_STATIC_S g_stPktStatic; /* ICMP */
struct timeval stSendTime ={ 0 }; /* ECHO-REQUEST */
struct timeval stRcvTime ={ 0 }; /* ECHO-REPLY */

/*  */
void showStatic(const ICMP_STATIC_S *pstStInfo)
{
	unsigned int uiSend, uiRecv;

	uiSend = pstStInfo->uiSendPktNum;
	uiRecv = pstStInfo->uiRcvPktNum;
	printf("\n***PING Statistics***");
	printf("\nPackets:Send = %u,Recveived = %u,Lost = %u", uiSend, uiRecv,
			uiSend - uiRecv);
	printf("\nTime:Minimum = %.1fms,Maximum = %.1fms,Average=%.2fms\n",
			pstStInfo->fMinTime, pstStInfo->fMaxTime, pstStInfo->fArgTime);
}

/* */
unsigned int timeSub(const struct timeval *pstOut, const struct timeval *pstIn)
{
	unsigned int uiSec = 0;
	int iUsec = 0;

	uiSec = pstOut->tv_sec - pstIn->tv_sec;
	iUsec = pstOut->tv_usec - pstIn->tv_usec;
	if (0 > iUsec)
	{
		iUsec += 1000000;
		uiSec--;
	}
	return uiSec * 1000 + (unsigned int) (iUsec / 1000);
}

/* */
unsigned short calcIcmpChkSum(const void *pPacket, int iPktLen)
{
	unsigned short usChkSum = 0;
	unsigned short *pusOffset = NULL;

	pusOffset = (unsigned short *) pPacket;
	while (1 < iPktLen)
	{
		usChkSum += *pusOffset++;
		iPktLen -= sizeof(unsigned short);
	}
	if (1 == iPktLen)
	{
		usChkSum += *((char *) pusOffset);
	}
	usChkSum = (usChkSum >> 16) + (usChkSum & 0xffff);
	usChkSum += (usChkSum >> 16);

	return ~usChkSum;
}

/* ICMP*/
int newIcmpEcho(const int iPacketNum, const int iDataLen)
{
	struct icmp *pstIcmp = NULL;

	memset(aucSendBuf, 0, sizeof(aucSendBuf));
	pstIcmp = (struct icmp *) aucSendBuf;
	pstIcmp->icmp_type = ICMP_ECHO;
	pstIcmp->icmp_code = 0;
	pstIcmp->icmp_seq = htons((unsigned short) iPacketNum);
	pstIcmp->icmp_id = htons((unsigned short) getpid());
	pstIcmp->icmp_cksum = 0;
	pstIcmp->icmp_cksum = calcIcmpChkSum(pstIcmp, iDataLen + 8);
	return iDataLen + 8;
}

/* ECHOREPLY*/
int parseIcmp(const struct sockaddr_in *pstFromAddr, char *pRecvBuf,
		const int iLen)
{
	int iIpHeadLen = 0;
	int iIcmpLen = 0;
	struct ip *pstIp = NULL;
	struct icmp *pstIcmpReply = NULL;

	pstIp = (struct ip *) pRecvBuf;
	iIpHeadLen = pstIp->ip_hl << 2;
	pstIcmpReply = (struct icmp *) (pRecvBuf + iIpHeadLen);
	/*  */
	iIcmpLen = iLen - iIpHeadLen;
	if (8 > iIcmpLen)
	{
		printf("[Error]Bad ICMP Echo-reply\n");
		return -1;
	}
	/*  */
	if ((pstIcmpReply->icmp_type != ICMP_ECHOREPLY)
			|| (pstIcmpReply->icmp_id != htons((unsigned short) getpid())))
	{
//		printf("other:%d,%d,%d,%d,%d\n",ntohs(pstIcmpReply->icmp_seq),pstIcmpReply->icmp_type , ICMP_ECHOREPLY,pstIcmpReply->icmp_id , htons((unsigned short) getpid()));
		return -2;
	}
//	sleep(1);
	printf("%d bytes reply from %s: icmp_seq=%u Time=%dms TTL=%d\n", iIcmpLen,
			inet_ntoa(pstFromAddr->sin_addr), ntohs(pstIcmpReply->icmp_seq),
			timeSub(&stRcvTime, &stSendTime), pstIp->ip_ttl);

	return 1;
}

/* Echo */
void recvIcmp(const int fd)
{
	int iRet = 0;
	int iRecvLen = 0;
	unsigned int uiInterval = 0;
	socklen_t fromLen = sizeof(struct sockaddr_in);
	struct sockaddr_in stFromAddr =
	{ 0 };

	/* �� */
	do
	{

		memset(aucRecvBuf, 0, 1024 * 1024);
		iRecvLen = recvfrom(fd, (void *) aucRecvBuf, sizeof(aucRecvBuf), 0,
				(struct sockaddr *) &stFromAddr, &fromLen);
//		printf("receive new data %d\n",iRecvLen);
		gettimeofday(&stRcvTime, NULL);
		if (0 > iRecvLen)
		{
			if (EAGAIN == errno)
			{
				/* */
				LOG("Ping %s Request time out.\n",DSTHOST);
				g_stPktStatic.fMaxTime = ~0;
			}
			else
			{
				/*  */
				perror("[Error]ICMP Receive");
			}
//			printf("recv 0");
			return;
		}
		/*   */

		uiInterval = timeSub(&stRcvTime, &stSendTime);
		g_stPktStatic.fArgTime = (g_stPktStatic.fArgTime
				* (g_stPktStatic.uiSendPktNum - 1) + uiInterval)
				/ g_stPktStatic.uiSendPktNum;
		if (uiInterval < g_stPktStatic.fMinTime)
		{
			g_stPktStatic.fMinTime = uiInterval;
		}
		if (uiInterval > g_stPktStatic.fMaxTime)
		{
			g_stPktStatic.fMaxTime = uiInterval;
		}
		/* ICMP */
		iRet = parseIcmp(&stFromAddr, (char *) aucRecvBuf, iRecvLen);
		if (-2 == iRet)
		{
			continue;
		}else if(1 == iRet){
//			printf("success \n");
			g_stPktStatic.uiRcvPktNum++;
		}
		break;
	}while(1);
}

/* ICMP */
void sendIcmp(const int fd, const struct sockaddr_in *pstDestAddr)
{
	unsigned char ucEchoNum = g_stPktStatic.uiSendPktNum;
	int iPktLen = 0;
	int iRet = 0;

//	while (ICMP_ECHO_MAX > ucEchoNum)
//	while(sem_trywait(&m_hEvent))
	do
	{
		iPktLen = newIcmpEcho(ucEchoNum, ICMP_DATA_LEN);
		/*  */
		g_stPktStatic.uiSendPktNum++;
		gettimeofday(&stSendTime, NULL);
		/* ICMPECHO */
//		printf("send [%d]",ucEchoNum);
		iRet = sendto(fd, aucSendBuf, iPktLen, 0,
				(struct sockaddr *) pstDestAddr, sizeof(struct sockaddr_in));
		if (0 > iRet)
		{
			perror("Send ICMP Error");
			continue;
		}
		/*  */
		recvIcmp(fd);
//		ucEchoNum++;
	}while(0);
}

int setip( char const * if_name ,const char * ipaddr)
{
    int sock_set_ip;

    struct sockaddr_in sin_set_ip;
    struct ifreq ifr_set_ip;

    bzero( &ifr_set_ip,sizeof(ifr_set_ip));

    if( ipaddr == NULL )
        return -1;

    sock_set_ip = socket( AF_INET, SOCK_STREAM, 0 );
    if(sock_set_ip < 0)
    {
        LOG("NetdeviceInfo::SetIP,socket create fail!");
       return -1;
    }

    memset( &sin_set_ip, 0, sizeof(sin_set_ip));
    strncpy(ifr_set_ip.ifr_name, if_name, sizeof(ifr_set_ip.ifr_name)-1);

    sin_set_ip.sin_family = AF_INET;
    sin_set_ip.sin_addr.s_addr = inet_addr(ipaddr);
    memcpy( &ifr_set_ip.ifr_addr, &sin_set_ip, sizeof(sin_set_ip));

    if( ioctl( sock_set_ip, SIOCSIFADDR, &ifr_set_ip) < 0 )
    {
        return -1;
    }

    //设置激活标志
    ifr_set_ip.ifr_flags |= IFF_UP |IFF_RUNNING;

    //get the status of the device
    if( ioctl( sock_set_ip, SIOCSIFFLAGS, &ifr_set_ip ) < 0 )
    {
         perror("");
         return -1;
    }

    close( sock_set_ip );
    return 0;
}
/**/
int selfping(const char *ipaddr)
{
	int iRet = 0;
	int iRcvBufSize = 1024 * 1024;
	int fd = E_FAILD_FD;
	in_addr_t stHostAddr;

	struct timeval stRcvTimeOut =	{ 0 };
	struct hostent *pHost = NULL;
	struct sockaddr_in stDestAddr =	{ 0 };
	struct protoent *pProtoIcmp = NULL;

	/* ICMP */
	pProtoIcmp = getprotobyname("icmp");
	if (NULL == pProtoIcmp)
	{
		perror("[Error]Get ICMP Protoent Structrue\n");
		return -1;
	}
	/* ICMPSOCKET */
	fd = socket(PF_INET, SOCK_RAW, pProtoIcmp->p_proto);
	if (0 > fd)
	{
		perror("[Error]Init Socket");
		return -1;
	}
	/* ROOT  */
	iRet = setuid(getuid());
	if (0 > iRet)
	{
		perror("[Error]Setuid");
		close(fd);
		return -1;
	}
	/* ����SOCKETѡ�� */
	stRcvTimeOut.tv_sec = ICMP_REQUEST_TIMEOUT;
	setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &iRcvBufSize, sizeof(iRcvBufSize));
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &stRcvTimeOut,
			sizeof(struct timeval));

	/*  */
	stHostAddr = inet_addr(ipaddr);
	if (INADDR_NONE == stHostAddr)
	{
		/* IP */
		pHost = gethostbyname(ipaddr);
		if (NULL == pHost)
		{
			perror("[Error]Host Name Error");
			close(fd);
			return -1;
		}
		memcpy((char *) &stDestAddr.sin_addr, (char *) (pHost->h_addr), pHost->h_length);
	}
	else
	{
		memcpy((char *)&stDestAddr.sin_addr, (char *)&stHostAddr, sizeof(stHostAddr));
	}
//	printf("\nPING %s(%s): %d bytes in ICMP packetsn\n", ipaddr,
//			inet_ntoa(stDestAddr.sin_addr), ICMP_DATA_LEN);
	/*ICMP*/
	sendIcmp(fd, &stDestAddr);

	/*  */
//	showStatic(&g_stPktStatic);
	/* FD */
	close(fd);
	return 0;
}

