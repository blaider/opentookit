/*************************************************************************
	> File Name: log.h
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Fri 30 Jan 2015 01:52:17 PM CST
 ************************************************************************/

#ifndef LOG_H
#define LOG_H

/*  */
typedef struct tagIcmpStatic
{
	unsigned int uiSendPktNum;
	unsigned int uiRcvPktNum;
	float fMinTime;
	float fMaxTime;
	float fArgTime;
} ICMP_STATIC_S;

/*  */
extern ICMP_STATIC_S g_stPktStatic; /* ICMP */

extern char g_logfile[];
void printRed(const char *msg);
void printGreen(const char *msg);
void printPass();
void printFailed();
void LOG(const char* ms, ...);
void LOGNOTIME(const char* ms, ...);

int selfping(const char *ipaddr);
int setip( char const * if_name ,const char * ipaddr);
#endif


