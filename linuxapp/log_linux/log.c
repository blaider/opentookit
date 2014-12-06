/*
 * =====================================================================================
 *
 *       Filename:  log.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年12月01日 22时42分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

static inline void LOG(const char* ms, ... )
{
	char wzLog[1024] = {0};
	char buffer[1024] = {0};
	va_list args;
	va_start(args, ms);
	vsprintf( wzLog ,ms,args);
	va_end(args);

	struct timeval tv;
	gettimeofday(&tv,NULL);
	struct tm *local;
	local = localtime(&tv.tv_sec);
	sprintf(buffer,"%04d-%02d-%02d %02d:%02d:%02d.%06ld %s\n", local->tm_year+1900, local->tm_mon,
				local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec,tv.tv_usec,
				wzLog);
	FILE* file = fopen("testResut.log","a+");
	fwrite(buffer,1,strlen(buffer),file);
	fclose(file);

	pid_t parent = getppid();
	if( parent == 2 || parent == 3)
		syslog(LOG_INFO,"%s",wzLog);
	else
		printf("%s",buffer);
	return ;
}
int main(int argc,char *argv[])
{
	if(argc > 1)
	{
		if(strcmp(argv[1],"-d") == 0)
		{
			daemon(0,0);
		}
	}
	pid_t parent = getppid();
	LOG("parent = %d",parent);
	return 0;
}
