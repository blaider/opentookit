/*************************************************************************
	> File Name: main.cpp
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Tue 30 Jan 2018 10:04:24 AM CST
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<iostream>
#include "Log.h"
using namespace std;

char szPath[PATH_MAX];
char szPathSuccess[PATH_MAX];
char szPathFailed[PATH_MAX];
char log_file[PATH_MAX];
FILE *fp_log = NULL;


int get_program_path(char *path)
{
	int i = 0;
	if(path == NULL)
		return -1;
	char szPath[PATH_MAX];
	size_t rslt = readlink("/proc/self/exe", szPath, sizeof(szPath) - 1);
	if (rslt < 0 || (rslt >= sizeof(szPath) - 1)) {
		return -1;
	}
	szPath[rslt] = '\0';
	for (i = rslt; i >= 0; i--) {
		if (szPath[i] == '/') {
			szPath[i + 1] = '\0';
			break;
		}
	}
	strcpy(path,szPath);

	sprintf(szPathSuccess,"%ssuccess/",path);
	if(access(szPathSuccess,F_OK))
	{
		mkdir(szPathSuccess,S_IRWXU|S_IRGRP|S_IROTH);
	}
	sprintf(szPathFailed,"%sfailed/",path);
	if(access(szPathFailed,F_OK))
	{
		mkdir(szPathFailed,S_IRWXU|S_IRGRP|S_IROTH);
	}

	return 0;
}

int check_sram(int count)
{
	int ram_count = 1;
	if(access("/dev/sram",F_OK))
		return -1;
	int fd = open("/dev/sram",O_RDWR);
	if(fd <= 0)
	{
		perror("open /dev/sram failed");
		LOGEX("/dev/sram open failed!\n");
		return -EINVAL;
	}
	if(count != 1 )
	{
		lseek(fd,0,SEEK_SET);
		read(fd,&ram_count,4);
	}

	lseek(fd,0,SEEK_SET);
	write(fd,&count,4);
	close(fd);
	LOGEX("ram count=%d,count=%d,",ram_count,count);
	if((ram_count != (count - 1)) && count != 1 )
	{
		LOGEX("ram test failed!\n");
		return -1;
	}

	LOGEX("ram test OK!\n");
	return 0;
}

int get_log_file()
{
	int errors = 0;
	char path[PATH_MAX];
	int count = 0;

	sprintf(path,"%scount",szPath);

	char buf[1024] = {0};
	int ret = 0;
	FILE *fp = fopen(path,"a+");
	if(fp == NULL)
		return -1;
	ret = fread(buf,1,sizeof(buf),fp);
	if(ret > 0)
		sscanf(buf,"%d",&count);
	count++;
	fclose(fp);
	fp = fopen(path,"w+");
	sprintf(buf,"%d",count);
	fwrite(buf,1,strlen(buf),fp);
	fclose(fp);

	struct timeval tv;
	gettimeofday(&tv,NULL);
	struct tm *local;
	local = localtime(&tv.tv_sec);
	sprintf(log_file,"%s%06d_%04d-%02d-%02d_%02d%02d%02d.log", szPath,count,local->tm_year+1900, 1 + local->tm_mon,
				local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
	fp_log = fopen(log_file,"w+");
	if(fp_log != NULL)
	{
		printf("Log File:%s\n",log_file);
	}
	if (check_sram(count) != 0)
		errors++;
//	fwrite(buffer,1,strlen(buffer),file);
//	fclose(file);
	return -errors;
}

//0 failed,1 success,2 not enough time
int system_ping(const char *ifname,char * urls)
{

	int i;
	int ret = -1;
	char cmd[2056];
	sprintf(cmd,"ping %s -c 1 -W 10 -s 8  -I %s",urls,ifname);
	printf("%s:%s\n",__func__,cmd);

	for (i = 0; i < 4; i++)
	{
		ret = system(cmd);
		printf("%s:ret=%d\n", __func__, ret);
		if (!WEXITSTATUS(ret))
		{
			LOGEX("[%s]ping %s success[%d]\n",ifname,urls,i+1);
			break;
		}
		sleep(5);
	}

	if(ret)
	{
		LOGEX("[%s]ping %s failed\n",ifname,urls);
	}
	return !ret;
}

int network_test()
{
	char buf[1024];
	char ifname[32];
	char ipaddr[64];
	int errors = 0;
	char configfile[PATH_MAX];
	sprintf(configfile,"%snetwork",szPath);
	if(access(configfile,F_OK))
	{
		FILE *fp = fopen(configfile,"a+");
		fclose(fp);
		return 0;
	}

	FILE *fp = fopen(configfile,"r");
	if(fp == NULL)
		return 0;
	while(fgets(buf,sizeof(buf),fp) != NULL)
	{
		int ret = sscanf(buf,"%s %s",ifname,ipaddr);
		if(ret == 2)
		{
			if(system_ping(ifname,ipaddr) == 0)
				errors++;
		}
	}
	fclose(fp);

	return -errors;
}

int comtest(char *coma,char * comb);
int moduletest();
int main(int argc,char *argv[])
{
	if(argc == 2)
	{
		if(strcmp("-d",argv[1]) == 0)
			daemon(0,0);
	}
	time_t time_old,time_new;
	time_old = time(NULL);

	char cmd[PATH_MAX];
	int errors = 0;
	get_program_path(szPath);
	if(get_log_file() != 0)
		errors++;

//	if(comtest((char *)"/dev/ttyAP0",(char *)"/dev/ttyAP3"))
//		errors++;
//	if(comtest((char *)"/dev/ttyAP3",(char *)"/dev/ttyAP0"))
//		errors++;
//	if(comtest((char *)"/dev/ttyAP1",(char *)"/dev/ttyAP2"))
//		errors++;
//	if(comtest((char *)"/dev/ttyAP2",(char *)"/dev/ttyAP1"))
//		errors++;

	if(moduletest())
		errors++;

//	if(network_test())
//		errors++;

	time_new = time(NULL);
	LOGEX("Test cost %5ld seconds\n",time_new - time_old);
	if(fp_log != NULL)
	{
		fclose(fp_log);
		fp_log = NULL;
	}

	if(errors)
	{
		sprintf(cmd,"mv %s %s",log_file,szPathFailed);
	}else{
		sprintf(cmd,"mv %s %s",log_file,szPathSuccess);
	}
	system(cmd);

	return 0;
}
