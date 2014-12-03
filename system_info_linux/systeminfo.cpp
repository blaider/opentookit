// #############################################################################
// *****************************************************************************
//                  Copyright (c) 2014, Advantech Automation Corp.
//      THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY
//               INFORMATION WHICH IS THE PROPERTY OF ADVANTECH AUTOMATION CORP.
//
//    ANY DISCLOSURE, USE, OR REPRODUCTION, WITHOUT WRITTEN AUTHORIZATION FROM
//               ADVANTECH AUTOMATION CORP., IS STRICTLY PROHIBITED.
// *****************************************************************************
// #############################################################################
//
// File:   	 systeminfo.cpp
// Author:  suchao.wang
// Created: Oct 27, 2014
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#include "systeminfo.h"
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <errno.h>
#include <sys/mount.h>
#include <dlfcn.h>
#include <pthread.h>
#include <semaphore.h>

#include "Log.h"
//nodeid
/** board resource(Watchdog/LED) handle */
	typedef int BR_HANDLE;

	/** The error code of BoardResource SDK */
	typedef enum board_resource_result
	{
		/** success */
		BR_SUCCESS = 0,
		/** device not exist */
		BR_DEVICE_NOT_EXIST = -1,
		/** device already opened */
		BR_DEVICE_ALREADY_OPENED = -2,
		/** the handle not opened */
		BR_ERR_DEVICE_NOT_OPENED = -3,
		/** i2c error */
		BR_ERR_I2C = -3,
		/** the i2c bus not exist */
		BR_ERR_I2C_BUS_NOT_EXIST = -4,
		/** the i2c device does not exist */
		BR_ERR_I2C_DEVICE_NOT_EXIST = -5,
		/** read the i2c command status error */
		BR_ERR_I2C_CONFIRM = -6,
		/** out of the parameter range */
		BR_ERR_OUT_RANGE = -7,
	} BR_RESULT;
	BR_RESULT (*Board_Init)( BR_HANDLE * handle );
	BR_RESULT (*Board_GetNodeID)( BR_HANDLE handle, unsigned int * nodeID );

//icdm
int (*IcdmLibInitialize)(void);
int (*IcdmLibUninitialize)(void);
unsigned int (*IcdmGetSerialPortCount)();
unsigned int (*IcdmGetLANPortCount)();
int (*IcdmGetSerialRateConfig)( unsigned int portNumber,
	unsigned int& mode,
	unsigned int& AStandard,
	unsigned int& BStandard,
	unsigned int& AMax,
	unsigned int& BMax,
	unsigned int& Enable );
int (*IcdmGetLANLinkStatus)( unsigned int  portNumber );
int (*IcdmGetAllRate)(unsigned char *pSerialRate,
	unsigned int serialCount,
	unsigned char  *pLanRate,
	unsigned int lanCount);

double g_cpu_loading;
sem_t sem;
void* calCpuLoadingThread( void* arg );

#define PROC_SUPER_MAGIC 0x9fa0
#define PROCFS "/proc"
int sysinfo_init()
{
	struct statfs sb;
	if (statfs(PROCFS, &sb) < 0 || sb.f_type != PROC_SUPER_MAGIC)
	{
		return -1;
	}

	pthread_t m_hThread;
	sem_init(&sem,0,1);
	pthread_create(&m_hThread, NULL, calCpuLoadingThread,NULL);

	void *dp;
	char *error;
	dp = dlopen("libBoardResource.so", RTLD_LAZY);
	if (dp == NULL)
	{
		fputs(dlerror(), stderr);
		return -1;
	}

	*(void **)&Board_Init = dlsym(dp, "Board_Init");
	error = dlerror();
	if (error)
	{
		fputs(error, stderr);
		return -1;
	}

	*(void **)&Board_GetNodeID = dlsym(dp, "Board_GetNodeID");
	error = dlerror();
	if (error)
	{
		fputs(error, stderr);
		return -1;
	}

	dp = dlopen("libicdm.so", RTLD_LAZY);
	if (dp == NULL)
	{
		fputs(dlerror(), stderr);
		return -1;
	}

	*(void **)&IcdmLibInitialize = dlsym(dp, "IcdmLibInitialize");
	error = dlerror();
	if (error)
	{
		fputs(error, stderr);
		return -1;
	}

	*(void **)&IcdmLibUninitialize = dlsym(dp, "IcdmLibUninitialize");
	error = dlerror();
	if (error)
	{
		fputs(error, stderr);
		return -1;
	}

	*(void **)&IcdmGetLANPortCount = dlsym(dp, "IcdmGetLANPortCount");
	error = dlerror();
	if (error)
	{
		fputs(error, stderr);
		return -1;
	}
	*(void **) &IcdmGetSerialPortCount = dlsym(dp, "IcdmGetSerialPortCount");
	error = dlerror();
	if (error)
	{
		fputs(error, stderr);
		return -1;
	}
	*(void **) &IcdmGetSerialRateConfig = dlsym(dp, "IcdmGetSerialRateConfig");
	error = dlerror();
	if (error)
	{
		fputs(error, stderr);
		return -1;
	}
	*(void **) &IcdmGetLANLinkStatus = dlsym(dp, "IcdmGetLANLinkStatus");
	error = dlerror();
	if (error)
	{
		fputs(error, stderr);
		return -1;
	}
	*(void **) &IcdmGetAllRate = dlsym(dp, "IcdmGetAllRate");
	error = dlerror();
	if (error)
	{
		fputs(error, stderr);
		return -1;
	}

	IcdmLibInitialize();


	return 0;
}

int get_cpu_jiffers(double *total,double *used)
{
	double user,nice,system,idle,iowait,irq,softirq;
	FILE * fd;
	char line[1024];
	char name[100];
	fd = fopen("/proc/stat", "r");
	if(fd == NULL)
	{
		perror("/proc/stat");
		return -1;
	}
	while (fgets(line, sizeof(line), fd))
	{
		int ret = sscanf(line, "%[^ ] %lf %lf %lf %lf %lf %lf %lf %*[^\n ]"
				, name,&user,&nice,&system,&idle,&iowait,&irq,&softirq);
		if (strcmp(name, "cpu") == 0 && ret == 8)
		{
			*total = user+nice+system+idle+iowait+irq+softirq;
			*used = user+nice+system+iowait+irq+softirq;
			break;
		}
	}
	fclose(fd);
	return 0;
}


void* calCpuLoadingThread( void* arg )
{
	double oldTotal = 0,oldUsed=0;
	double newTotal = 0,newUsed= 0;
	get_cpu_jiffers(&oldTotal,&oldUsed);
	while(1)
	{
		sleep(1);
		get_cpu_jiffers(&newTotal,&newUsed);
		sem_wait(&sem);
		g_cpu_loading = (newUsed - oldUsed)/(newTotal - oldTotal)*100;
		sem_post(&sem);
		oldTotal = newTotal;
		oldUsed = newUsed;
	}
	return NULL;
}


int get_system_cpu_freq( double *freq )
{
	*freq = 600*1024*1024;
	return 0;
}
int get_system_cpu_used(double *used)
{
	if (used == NULL)
		return -1;
	sem_wait(&sem);
	*used = g_cpu_loading;
	sem_post(&sem);
//	struct statics stat;
//	machine_init(&stat);
//	struct system_info info;
//	get_system_info(&info);
//	*used = (float) info.cpustates[0] / 10 + (float) info.cpustates[1] / 10
//			+ (float) info.cpustates[2] / 10;
//	printf("%f\n", (float) info.cpustates[3] / 10);
	return 0;
}

int get_system_memory_size(double *total)
{
	if (total == NULL)
		return -1;
//	struct statics stat;
//	machine_init(&stat);
//	struct system_info info;
//	get_system_info(&info);
//	*total = info.memory[0];

	double ma;
	FILE * fd;
	char line[100];
	char name[100];
	fd = fopen("/proc/meminfo", "r");
	if(fd == NULL)
	{
		perror("/proc/meminfo");
		return -1;
	}
	while (fgets(line, sizeof(line), fd))
	{
		sscanf(line, "%[^:] %*[^ ]  %lf %*[^\n ]", name, &ma);
		if (strcmp(name, "MemTotal") == 0)
		{
			*total = ma * 1024;
			break;
		}
	}
	fclose(fd);
	return 0;
}

int get_system_memory_used(double *used)
{
	if (used == NULL)
		return -1;
//	struct statics stat;
//	machine_init(&stat);
//	struct system_info info;
//	get_system_info(&info);
//	*used = 100.0 * (info.memory[0] - info.memory[1]) / info.memory[0];
	double total = 0;
	double free = 0;
	double ma;
	FILE * fd;
	char line[100];
	char name[100];
	fd = fopen("/proc/meminfo", "r");
	if(fd == NULL)
	{
		perror("/proc/meminfo");
		return -1;
	}
	while (fgets(line, sizeof(line), fd))
	{
		sscanf(line, "%[^:] %*[^ ]  %lf %*[^\n ]", name, &ma);
		if (strcmp(name, "MemTotal") == 0)
		{
			total = ma;
		}
		else if (strcmp(name, "MemFree") == 0)
		{
			free = ma;
		}

		if (total > 0 && free > 0)
			break;
	}
	*used = (total - free) / total * 100;
	fclose(fd);
	return 0;
}

int get_system_uptime(double *time)
{
	if (time == NULL)
		return -1;


	/* chdir to the proc filesystem to make things easier */

	char buffer[4096 + 1];
	int fd, len;

	fd = open("/proc/uptime", O_RDONLY);
	if(fd < 0 )
	{
		perror("/proc/uptime");
		return -1;
	}
	len = read(fd, buffer, sizeof(buffer) - 1);
	close(fd);
	buffer[len] = '\0';

	*time = atof(buffer);
	return 0;
}

int get_system_utctime(double *times)
{
	if(times == NULL)
		return -1;
//	struct tm *local;
	time_t t;
	t = time(NULL);
	*times = t;
//	local=localtime(&t);
//	printf("Local hour is: %d\n",local->tm_hour);
//	local=gmtime(&t);
//	printf("UTC hour is: %d\n",local->tm_hour);
	return 0;
}

int get_system_tf_capacity(double *capacity)
{
	if (capacity == NULL)
		return -1;
	*capacity = 0;
	FILE *procpt;
	char line[100];
	double ma, mi;
	char tmp[4][100];

	procpt = popen("fdisk -l /dev/mmcblk0", "r");
	if(procpt == NULL)
	{
		perror("fdisk -l /dev/mmcblk0");
		return -1;
	}
	while (fgets(line, sizeof(line), procpt))
	{
		if (sscanf(line, "%[^ ] %[^ ] %lf %[^ ] %lf %[^\n ]", tmp[0], tmp[1],
				&mi, tmp[2], &ma, tmp[3]) != 6)
			continue;
		*capacity = ma;
		break;
	}
	pclose(procpt);
	return 0;
}

int get_system_tf_free(double *free)
{
	if(free == NULL)
			return -1;
    struct statfs diskInfo;
    statfs("/",&diskInfo);
    unsigned long long totalBlocks = diskInfo.f_bsize;
    unsigned long long freeDisk = diskInfo.f_bfree*totalBlocks;

    *free = freeDisk;
	return 0;
}

int get_system_sd_capacity(double *capacity)
{
	if (capacity == NULL)
		return -1;
	FILE *procpt;
	char line[100];
	double ma, mi;
	char tmp[4][100];
	*capacity = 0;

	procpt = popen("fdisk -l /dev/mmcblk1", "r");
	if(procpt == NULL)
	{
		perror("fdisk -l /dev/mmcblk1");
		return -1;
	}
	while (fgets(line, sizeof(line), procpt))
	{
		if (sscanf(line, "%[^ ] %[^ ] %lf %[^ ] %lf %[^\n ]", tmp[0], tmp[1],
				&mi, tmp[2], &ma, tmp[3]) != 6)
			continue;
		*capacity = ma;
		break;
	}
	pclose(procpt);
	return 0;
}

int get_system_sd_free(double *free)
{
	if (free == NULL)
		return -1;
	*free = 0;
	double diskTotal;
	get_system_sd_capacity(&diskTotal);
	if (diskTotal <= 1)
		return -2;
	struct statfs diskInfo;
	statfs("/media/mmcblk1p1", &diskInfo);
	unsigned long long totalBlocks = diskInfo.f_bsize;
	unsigned long long freeDisk = diskInfo.f_bfree * totalBlocks;

	*free = freeDisk;
    return 0;
}

int get_system_node_id(double *nodeid)
{
	if(nodeid == NULL)
		return -1;
	*nodeid = 0xffff;

	BR_HANDLE node_id_fd;
	unsigned int device_id;
	Board_Init(&node_id_fd);
	if (node_id_fd > 0)
	{
		Board_GetNodeID(node_id_fd,&device_id);
		*nodeid = device_id;
	}

	return 0;
}

int get_system_serial_count(double *count)
{
	if(count == NULL)
		return -1;
	*count = 0;
	if(IcdmGetSerialPortCount == NULL)
		return -1;
	*count = IcdmGetSerialPortCount();
//	for(int i=0;i<*count;i++)
//	{
//		double status,score;
//		get_icdm_serial_mode(&status,i);
//		get_icdm_serial_score(&score,i);
//		printf("uart%d,mode:%lf,rate:%lf\n",i,status,score);
//
//	}
	return 0;
}
int get_system_lan_count(double *count)
{
	if(count == NULL )
		return -1;
	*count = 0;
	if(IcdmGetLANPortCount == NULL)
		return -1;
	*count = IcdmGetLANPortCount();
//	for(int i=0;i<*count;i++)
//	{
//		double status,score;
//		get_icdm_lan_linkstatus(&status,i);
//		get_icdm_lan_score(&score,i);
//		printf("lan%d,linkstatus:%lf,rate:%lf\n",i,status,score);
//
//	}
	return 0;
}


int get_icdm_serial_mode(double *mode,int no)
{
	if(mode == NULL )
		return -1;
	*mode = 0;
	unsigned int imode,as,bs,am,bm,enable;
	IcdmGetSerialRateConfig(no + 1,imode,as,bs,am,bm,enable);
	*mode = imode;
	return 0;
}
int get_icdm_serial_score(double *score,int no)
{
	if(score == NULL )
		return -1;
	*score = 0;
	unsigned char lanBuffer[20];
	unsigned char uartBuffer[20];
	memset(lanBuffer,0,sizeof(lanBuffer));
	memset(uartBuffer,0,sizeof(uartBuffer));
	IcdmGetAllRate(uartBuffer,3,lanBuffer,2);
	*score = uartBuffer[no];
	if(*score > 100)
		*score = 0;
	return 0;
}
int get_icdm_lan_linkstatus(double *status,int no)
{
	if(status == NULL )
		return -1;
	*status = 0;
//	unsigned int lle;
//	unsigned char linkStatus,linkMode;
	*status = IcdmGetLANLinkStatus(no+1);
	return 0;
}
int get_icdm_lan_score(double *score,int no)
{
	if(score == NULL )
		return -1;
	*score = 0;
	unsigned char lanBuffer[20];
	unsigned char uartBuffer[20];
	memset(lanBuffer,0,sizeof(lanBuffer));
	memset(uartBuffer,0,sizeof(uartBuffer));
	IcdmGetAllRate(uartBuffer,3,lanBuffer,2);
	*score = lanBuffer[no];
	if(*score > 100)
		*score = 0;
	return 0;
}


