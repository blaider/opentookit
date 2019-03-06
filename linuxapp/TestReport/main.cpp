//============================================================================
// Name        : main.cpp
// Author      : suchao.wang
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++
//============================================================================


#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <limits.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/reboot.h>
#include <string.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include "AwcCommon.h"
#include "Log.h"

#include "AwcModule.h"
//
// Print a greeting message on standard output and exit.
//
// On embedded platforms this might require semi-hosting or similar.
//
// For example, for toolchains derived from GNU Tools for Embedded,
// to enable semi-hosting, the following was added to the linker:
//
// --specs=rdimon.specs -Wl,--start-group -lgcc -lc -lc -lm -lrdimon -Wl,--end-group
//
// Adjust it for other toolchains.

typedef struct _module_info
{
	char moduleName[32];
	void *data;

}MODULEINFO,*PMODULEINFO;

int Lockfile(const int iFd)
{
	struct flock    stLock;

	stLock.l_type = F_WRLCK;        /* F_RDLCK, F_WRLCK, F_UNLCK */
	stLock.l_start = 0;    /* byte offset, relative to l_whence */
	stLock.l_whence = SEEK_SET;    /* SEEK_SET, SEEK_CUR, SEEK_END */
	stLock.l_len = 0;        /* #bytes (0 means to EOF) */

	return (fcntl(iFd, F_SETLK, &stLock));
}
bool process_is_first_instance ( char const * proc_name )
{
	char pid_file_name[ PATH_MAX ];
	int pid_file;
	int ret;

	sprintf(
		pid_file_name,
		"/tmp/apal_proc_%s.pid",
		proc_name );

	pid_file = open(
		pid_file_name,
		O_CREAT | O_RDWR,
		0666 );
	if ( -1 == pid_file )
	{
		return 0;
	}

	ret = Lockfile( pid_file);
	if ( 0 == ret )
	{
		// this is the first instance
		return 1;
	}
	return 0;
}
#define APP_NAME "AdvWirelessCheck"

#ifndef VERSION_NUMBER
#define VERSION_NUMBER					""
#endif

static void print_app_version ( void )
{
#ifdef REVISION_NUMBER
	printf( "%s %s rev %s\n", APP_NAME, VERSION_NUMBER, REVISION_NUMBER );
#else
	printf( "%s %s build %s %s\n", APP_NAME, __DATE__,__TIME__ );
#endif
}

PAWC_CONNECTION getConnection(const char *filePath)
{
	void *handle = dlopen(filePath, RTLD_NOW);
	if (!handle) {
		LOGEX("open %s failed\nERROR:%s", filePath , dlerror());
		return NULL;
	}

	PAWC_CONNECTION wlan_conn = NULL;
	PAWC_CONNECTION (*GetConnection)();
	*(void **)&GetConnection = dlsym(handle, "GetConnection");
	if (GetConnection != 0) {
		wlan_conn = GetConnection();
		LOGEX( "Load module %s, netcard %s\n", filePath, wlan_conn->ifName);
	}else
	{
		LOGEX( "Get Module %s failed\n" , filePath);
	}
	return wlan_conn;
}


void* connProcessThread(void* arg)
{
	MODULEINFO *pinfo = (PMODULEINFO)arg;
	char mname[512];
	sprintf(mname,"%s%s",PRE_DIR,pinfo->moduleName);
	printf("%s,%s++++\n", __func__,mname);

	AWC_CONNECTION *conn;
	conn = getConnection(mname);
	if (!conn )
	{
		LOGEX("init %s failed ,quit checkConnection",mname);
		return NULL;
	}

	conn->loadConfig(conn,pinfo->data);

	time_t lastConnectionTime = time(NULL);

	return NULL;
}

int test()
{
	char szPath[PATH_MAX];
	char filePath[PATH_MAX];
	int i;
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	void *handle;

	get_program_path(szPath);
	printf("%s:%s\n",__func__,szPath);

	if ((dp = opendir(szPath)) == NULL) {
		fprintf(stderr, "cannot open directory: %s\n", szPath);
		return 0;
	}
	chdir(szPath);
	while ((entry = readdir(dp)) != NULL) {
		lstat(entry->d_name, &statbuf);
		if (S_ISREG(statbuf.st_mode)) {
			if ((strstr(entry->d_name, ".so") != NULL) && (strstr(entry->d_name, "mod_") != NULL))
			{
				/*
				 打开某个动态库，注意检查打开是否成功
				 */
				strcpy(filePath,szPath);
				strcat(filePath,entry->d_name);
				printf("%s:find %s\n",__func__,filePath);
			}
		}
	}
	closedir(dp);
}

int main(int argc, char ** argv)
{
	//Enable core_dump
	struct rlimit rlim;
	rlim.rlim_cur = 10*1024*1024;
	rlim.rlim_max = 10*1024*1024;
	setrlimit( RLIMIT_CORE, &rlim );
	//end enable core_dump
	if(argc > 1)
	{
		if (strcmp(argv[1], "--version") == 0)
		{
			print_app_version();
			return 0;
		}
		if (strcmp(argv[1], "-d") == 0)
		{
			daemon(0,0);
		}
	}

	int ret;
	ret = process_is_first_instance(APP_NAME);
	if (!ret)
	{
		LOGEX("%s already running\n", APP_NAME);
		exit(0);
	}

	test();

	return 0;
}
