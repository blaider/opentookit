//============================================================================
// Name        : main.cpp
// Author      : suchao.wang
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++
//============================================================================


#include <iostream>
#include <sys/stat.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
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
#include "AwcCommon.h"
#include "Log.h"
#include "DCTag.h"

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

sem_t sem_config;

static void int_hander(int s)
{
	printf("Catch a signal sigint\n");
}

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
	else
	{
		if ( EWOULDBLOCK == errno )
		{
//			APAL_LOG_WARN(
//				"%s: another instance is running with name %s\n",
//				__func__,
//				proc_name );
		}
		else
		{
//			APAL_LOG_ERROR(
//				"%s: flock return errno = %d with name %s\n",
//				__func__,
//				errno,
//				proc_name );
		}
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

enum comm_state
{
	COMM_STATE_IDLE,
	COMM_STATE_CHECK_REBOOT,
	COMM_STATE_CHECK_DEVICE,
	COMM_STATE_CHECK_PROCESS,
	COMM_STATE_CHECK_CONNECTION,
	COMM_STATE_REDEIAL,
	COMM_STATE_RESTART_DEVICE,
};
typedef enum comm_state				COMM_STATE;

#define conn_state_switch_to(ns) \
		do \
		{ \
			comm_state = ns; \
			printf("switch to %d\n",ns);\
		} while ( 0 )
#define RETRYTIMES 4
#define PRE_DIR "/home/root/bin/"


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
	sem_post(&sem_config);
	//reinit every 5 seconds
	while(!conn->init(conn))
	{
		LOGEX("init %s failed ,reinit in 5 seconds",mname);
		sleep(5);
	}

	time_t lastConnectionTime = time(NULL);
	COMM_STATE comm_state = COMM_STATE_IDLE;
	while (1)
	{
		switch(comm_state)
		{
		case COMM_STATE_IDLE:
			sleep(5);
			conn_state_switch_to(COMM_STATE_CHECK_REBOOT);
			break;
		case COMM_STATE_CHECK_REBOOT:
			conn_state_switch_to(COMM_STATE_CHECK_DEVICE);
			//check reboot setting
			if(!conn->checkRebootSystem(conn,lastConnectionTime))
			{
				LOGEX("reboot system in 5 seconds");
				 sync();
				 sync();
				 sleep(5);
				 system("killall wdtd");
				 reboot(RB_AUTOBOOT);
			}

			break;
		case COMM_STATE_CHECK_DEVICE:
		{
			//restart usb if the can't find device
			if (!conn->checkDevice(conn))
			{
				LOGEX("%s,%s:restartModem", __func__,pinfo->moduleName);
				conn->restartDevice(conn);
				conn_state_switch_to(COMM_STATE_IDLE);
			}else
				conn_state_switch_to(COMM_STATE_CHECK_PROCESS);
		}
			break;
		case COMM_STATE_CHECK_PROCESS:
		{
			//redial if the process is die
			if (!conn->checkProcess(conn))
			{
				LOGEX("%s,%s:dialUp", __func__,pinfo->moduleName);
				conn->dialUp(conn);
				conn_state_switch_to(COMM_STATE_IDLE);
			}else
				conn_state_switch_to(COMM_STATE_CHECK_CONNECTION);
		}
			break;
		case COMM_STATE_CHECK_CONNECTION:
		{
			conn_state_switch_to(COMM_STATE_REDEIAL);
			int i = 0;
			for (i = 0; i < RETRYTIMES; i++)
			{
				if (conn->checkConnection(conn))
				{
					lastConnectionTime = time(NULL);
					conn_state_switch_to(COMM_STATE_IDLE);
					break;
				}
				sleep(5);
			}
		}
			break;
		case COMM_STATE_REDEIAL:
		{
			conn_state_switch_to(COMM_STATE_RESTART_DEVICE);
			int i = 0;
			for (i = 0; i < RETRYTIMES; i++)
			{
				conn->dialDown(conn);
				sleep(5);
				if (conn->dialUp(conn))
				{
					conn_state_switch_to(COMM_STATE_IDLE);
					break;
				}
				sleep(5);
			}
		}
			break;
		case COMM_STATE_RESTART_DEVICE:
		{
			conn->restartDevice(conn);
			conn_state_switch_to(COMM_STATE_IDLE);
		}
			break;

		}

	}
	return NULL;
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

	signal(SIGUSR1, int_hander);

	pthread_t m_hThread,m_hThread2;
//	pthread_create(&m_hThread, NULL, connProcessThread, (void*)&minfo[0]);
//	pthread_create(&m_hThread2, NULL, connProcessThread, (void*)&minfo[1]);

	sem_init(&sem_config,0,0);

	const char name[] = "/home/root/project/SystemSetting.acr";
		if(access(name,F_OK))
			return false;
		xmlDocPtr doc = NULL;
		xmlNodePtr root = NULL;

		doc = xmlParseFile(name);
		if (doc == NULL)
		{
			return false;
		}
		root = xmlDocGetRootElement(doc);
		if (root == NULL)
		{
			xmlFreeDoc(doc);
			xmlCleanupParser();
			return false;
		}
		xmlNodePtr Config = NULL;
		Config = root->xmlChildrenNode;
		while (Config != NULL)
		{
			if (strcmp((const char*) Config->name, "LAN") == 0)
			{
				xmlNodePtr child = Config->xmlChildrenNode;
				while (child != NULL)
				{
					if (strcmp((const char*) child->name, "GPRS") == 0 )
					{
						xmlChar * connect,*is4G;//,*other;//,*other1,*other2;
						connect = xmlGetProp(child, BAD_CAST "connection");
						is4G = xmlGetProp(child, BAD_CAST "is4G");
						if (!strcmp((char *) connect, "true"))
						{
							MODULEINFO *pinfo = (PMODULEINFO)malloc(sizeof(MODULEINFO));
							if(is4G && !strcmp((char *)is4G,"true"))
								sprintf(pinfo->moduleName,"%s","awc_4g.so");
							else
								sprintf(pinfo->moduleName,"%s","awc_3g.so");
							pinfo->data = child;
							pthread_create(&m_hThread, NULL, connProcessThread, (void*)pinfo);
							sem_wait(&sem_config);
//							sleep(5);//wait for connectin init config
	//						conn->isEnable = true;
	//						gprs_enable=1;
						}

					}
					if (strcmp((const char*) child->name, "WiFi") == 0)
					{
						xmlChar *enables;
						enables = xmlGetProp(child, BAD_CAST "Enable");
						if (!strcmp((char *) enables, "true"))
						{
							MODULEINFO *pinfo = (PMODULEINFO)malloc(sizeof(MODULEINFO));
							sprintf(pinfo->moduleName,"%s","awc_wlan.so");
							pinfo->data = child;
							pthread_create(&m_hThread2, NULL, connProcessThread, (void*)pinfo);
							sem_wait(&sem_config);
//							sleep(5);//wait for connectin init config
						}else{
							if(!access("/etc/wpa_supplicant.conf",F_OK))
							{
								remove("/etc/wpa_supplicant.conf");
							}
						}
					}
					child = child->next;
				}
			}
			Config = Config->next;
		}
//	xmlFreeDoc(doc);
//	xmlCleanupParser();

	while(1)
	{
		sleep(5);
	}
	return 0;
}
