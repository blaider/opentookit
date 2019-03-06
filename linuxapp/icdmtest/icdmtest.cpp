/*************************************************************************
	> File Name: icdmtest.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Mon 06 Nov 2017 01:37:58 PM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <string.h>

//icdm
extern "C" typedef int (*ICDMLIBINITIALIZE)(void);
extern "C" typedef unsigned int (*ICDMGETSERIALPORTCOUNT)();
extern "C" typedef unsigned int (*ICDMGETLANPORTCOUNT)();
extern "C" typedef int (*ICDMGETSERIALRATECONFIGS)( unsigned int portNumber,
	unsigned int* mode,
	unsigned int* AStandard,
	unsigned int* BStandard,
	unsigned int* AMax,
	unsigned int* BMax,
	unsigned int* Enable );
extern "C" typedef int (*ICDMGETLANLINKSTATUS)( unsigned int  portNumber );
extern "C" typedef int (*ICDMGETALLRATE)(unsigned char *pSerialRate,
	unsigned int serialCount,
	unsigned char  *pLanRate,
	unsigned int lanCount);

static ICDMLIBINITIALIZE IcdmLibInitialize = NULL;
static ICDMGETSERIALPORTCOUNT IcdmGetSerialPortCount = NULL;
static ICDMGETLANPORTCOUNT IcdmGetLANPortCount = NULL;
static ICDMGETSERIALRATECONFIGS IcdmGetSerialRateConfigs = NULL;
static ICDMGETLANLINKSTATUS IcdmGetLANLinkStatus = NULL;
static ICDMGETALLRATE IcdmGetAllRate = NULL;

static int lib_icdm_init = 0;
//return 0 for success ,else for failed
int lib_icdm_load()
{
	printf("%s+++++\n",__func__);
	if( lib_icdm_init == 1 )
		return 0;

	void *dp;
	char *error;
	dp = dlopen("libicdm.so", RTLD_NOW);
	if (dp == NULL)
	{
		fputs(dlerror(), stderr);
		return -1;
	}

	IcdmLibInitialize = (ICDMLIBINITIALIZE)dlsym(dp, "IcdmLibInitialize");
	error = dlerror();
	if (error)
	{
		fputs(error, stderr);
		return -1;
	}

	IcdmGetLANPortCount = (ICDMGETLANPORTCOUNT)dlsym(dp, "IcdmGetLANPortCount");
	error = dlerror();
	if (error)
	{
		fputs(error, stderr);
		return -1;
	}
	IcdmGetSerialPortCount = (ICDMGETSERIALPORTCOUNT)dlsym(dp, "IcdmGetSerialPortCount");
	error = dlerror();
	if (error)
	{
		fputs(error, stderr);
		return -1;
	}
	IcdmGetSerialRateConfigs = (ICDMGETSERIALRATECONFIGS)dlsym(dp, "IcdmGetSerialRateConfigs");
	error = dlerror();
	if (error)
	{
		fputs(error, stderr);
		return -1;
	}
	IcdmGetLANLinkStatus = (ICDMGETLANLINKSTATUS)dlsym(dp, "IcdmGetLANLinkStatus");
	error = dlerror();
	if (error)
	{
		fputs(error, stderr);
		return -1;
	}
	IcdmGetAllRate = (ICDMGETALLRATE)dlsym(dp, "IcdmGetAllRate");
	error = dlerror();
	if (error)
	{
		fputs(error, stderr);
		return -1;
	}

	IcdmLibInitialize();

	lib_icdm_init = 1;
	printf("%s------\n",__func__);
	return 0;
}

extern "C" int main ( int argc, char* argv[] )
{
//	daemon(0,0);
	int fd[5];
	long fpos;
	fd[0] = open("open_file_test0", O_RDWR | O_CREAT, 0644);
	fd[1] = open("open_file_test1", O_RDWR | O_CREAT, 0644);
	fd[2] = open("open_file_test2", O_RDWR | O_CREAT, 0644);
	fd[3] = open("open_file_test3", O_RDWR | O_CREAT, 0644);
	fd[4] = open("open_file_test4", O_RDWR | O_CREAT, 0644);
	lib_icdm_load();
	char buf[20];
	int i = 0;
	while(1)
	{
		int ret = 0;
		sprintf(buf, "%d\n", i++);
		ret = write(fd[0], buf, strlen(buf));
		if (ret < 0)
			printf("error:0\n");
		ret = write(fd[1], buf, strlen(buf));
		if (ret < 0)
			printf("error:1\n");
		ret = write(fd[2], buf, strlen(buf));
		if (ret < 0)
			printf("error:2\n");
		ret = write(fd[3], buf, strlen(buf));
		if (ret < 0)
			printf("error:3\n");
		ret = write(fd[4], buf, strlen(buf));
		if (ret < 0)
			printf("error:4\n");
		fsync(fd[0]);
		fsync(fd[1]);
		fsync(fd[2]);
		fsync(fd[3]);
		fsync(fd[4]);
		fsync(fd[5]);
		sleep(5);
	}
}
