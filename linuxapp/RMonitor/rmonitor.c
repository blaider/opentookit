/*************************************************************************
	> File Name: rmonitor.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Fri 23 Jun 2017 04:06:23 PM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<syslog.h>
#include <string.h>
#include <stdarg.h>

static double mem_slab = 0;
static double mem_slab_r = 0;
static double mem_slab_ur = 0;

int get_system_memory_slab()
{

	double ma;
	FILE * fd;
	char line[100];
	char name[100];
	fd = fopen("/proc/meminfo", "r");
	if(fd == NULL)
	{
		perror("/proc/meminfo");
		return 0;
	}
	while (fgets(line, sizeof(line), fd))
	{
		sscanf(line, "%[^:] %*[^ ]  %lf %*[^\n ]", name, &ma);
		if (strcmp(name, "Slab") == 0)
		{
			mem_slab = ma * 1024;
		}else
		if (strcmp(name, "SUnreclaim") == 0)
		{
			mem_slab_ur = ma * 1024;
		}else
		if (strcmp(name, "SReclaimable") == 0)
		{
			mem_slab_r = ma * 1024;
		}
	}
	fclose(fd);
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
		return 0;
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
int g_fd_open = 0;
int get_fd_opend()
{
	unsigned int used,left,total;
	FILE * fd;
	char line[1024];
	fd = fopen("/proc/sys/fs/file-nr", "r");
	if(fd == NULL)
	{
		perror("/proc/sys/fs/file-nr");
		return 0;
	}
	while (fgets(line, sizeof(line), fd))
	{
		int ret = sscanf(line, "%d %d %d",&used,&left,&total);
		if (ret == 3)
		{
			g_fd_open = used;
			break;
		}
	}
	fclose(fd);
	return 0;
}

double oldTotal = 0,oldUsed=0;
double newTotal = 0,newUsed= 0;
static double g_cpu_loading;
static void var_init()
{
	get_cpu_jiffers(&oldTotal,&oldUsed);
}
static void get_cpu_used()
{
	get_cpu_jiffers(&newTotal,&newUsed);
	g_cpu_loading = (newUsed - oldUsed)/(newTotal - oldTotal)*100;
	oldTotal = newTotal;
	oldUsed = newUsed;
}
double mem_used;
int get_system_memory_used(double *used)
{
	if (used == NULL)
		return 0;
	double total = 0;
	double free = 0;
	double cached = 0;
	double ma;
	FILE * fd;
	char line[100];
	char name[100];
	fd = fopen("/proc/meminfo", "r");
	if(fd == NULL)
	{
		perror("/proc/meminfo");
		return 0;
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
		}else if (strcmp(name, "Cached") == 0)
		{
			cached = ma;
		}

		if (total > 0 && free > 0 && cached > 0)
			break;
	}
	*used = (total - free - cached) / total * 100;
	fclose(fd);
	return 0;
}
void LOG(const char* wzMsgFormat, ...)
{
	char wzLog[4096] = {0};
	va_list args;
	va_start(args, wzMsgFormat);
	vsprintf( wzLog ,wzMsgFormat,args);
	va_end(args);
	syslog(LOG_INFO,wzLog);
	printf("%s",wzLog);
}
static void showMessage()
{
	LOG("CPU:%10lf%, Mem:%10lf%,slab:%7.0lf KB[R%7.0lf+UR%7.0lf], FD Opened:%5d\n"
			,g_cpu_loading,mem_used,mem_slab/1024,mem_slab_r/1024,mem_slab_ur/1024,g_fd_open);
}
void* calCpuLoadingThread( )
{

	var_init();
	while(1)
	{
		sleep(1);
		get_cpu_used();
		get_system_memory_used(&mem_used);
		get_fd_opend();
		get_system_memory_slab();
		showMessage();
	}
	return NULL;
}

int main(int argc,char *argv[])
{
	if(argc > 1)
	{
		if(strcmp(argv[1],"-d") == 0)
			daemon(0,0);
	}
	calCpuLoadingThread();
	return 0;
}
