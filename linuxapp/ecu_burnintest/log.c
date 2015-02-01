/*************************************************************************
	> File Name: log.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Fri 30 Jan 2015 01:59:23 PM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <syslog.h>
#include <string.h>

char g_logfile[256] = {0};
void printRed(const char *msg)
{
	printf("\033[40;31m%s\033[0m",msg);
	return;
}
void printGreen(const char *msg)
{
	printf("\033[40;32m%s\033[0m",msg);
	return ;
}
void printPass()
{
	printGreen("\n");
	printGreen("\t*******          *           ******        ******   \n");
	printGreen("\t*      *        * *         *             *         \n");
	printGreen("\t*      *       *   *        *             *         \n");
	printGreen("\t*******       *     *         ******       ******   \n");
	printGreen("\t*            *********              *            *  \n");
	printGreen("\t*           *         *             *            *  \n");
	printGreen("\t*          *           *     *******       ******   \n");
	printGreen("\n");
	return;
}
void printFailed()
{
	printRed("\n");
	printRed("\t********        *         ***    *         ********   *********    \n");
	printRed("\t*              * *         *     *         *          *        * \n");
	printRed("\t*             *   *        *     *         *          *         *\n");
	printRed("\t*******      *     *       *     *         ********   *         * \n");
	printRed("\t*           *********      *     *         *          *         *  \n");
	printRed("\t*          *         *     *     *         *          *        * \n");
	printRed("\t*         *           *   ***    ********  ********   *********  \n");
	printRed("\n");
	return ;
}

void LOG(const char* ms, ...)
{
	char wzLog[1024] =
	{ 0 };
	char buffer[1024] =
	{ 0 };
	time_t now;
	struct tm *local;
	FILE *file;

	va_list args;
	va_start(args, ms);
	vsprintf(wzLog, ms, args);
	va_end(args);

	time(&now);
	local = localtime(&now);
	sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d %s", local->tm_year + 1900,
			local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min,
			local->tm_sec, wzLog);
	printf("%s",buffer);
	if(strlen(g_logfile) < 1)
		sprintf(g_logfile,"testResult%04d-%02d-%02d_%02d_%02d_%02d.log",local->tm_year+1900,local->tm_mon +1,local->tm_mday,local->tm_hour,local->tm_min,local->tm_sec);
	file = fopen(g_logfile, "a+");
	if(NULL == file)
		return ;
	fwrite(buffer, 1, strlen(buffer), file);
	fclose(file);
	sync();

//	syslog(LOG_INFO,wzLog);
	return;
}

void LOGNOTIME(const char* ms, ...)
{
	char wzLog[1024] =
	{ 0 };
	char buffer[1024] =
	{ 0 };
	FILE *file;
	time_t now;
	struct tm *local;

	va_list args;
	va_start(args, ms);
	vsprintf(wzLog, ms, args);
	va_end(args);

	time(&now);
	local = localtime(&now);
	sprintf(buffer, "%s",wzLog);
	printf("%s",buffer);
	if(strlen(g_logfile) < 1)
		sprintf(g_logfile,"testResult%04d-%02d-%02d_%02d_%02d_%02d.log",local->tm_year+1900,local->tm_mon +1,local->tm_mday,local->tm_hour,local->tm_min,local->tm_sec);
	file = fopen(g_logfile, "a+");
	if(NULL == file)
		return ;
	fwrite(buffer, 1, strlen(buffer), file);
	fclose(file);
	sync();

//	syslog(LOG_INFO,wzLog);
	return;
}
