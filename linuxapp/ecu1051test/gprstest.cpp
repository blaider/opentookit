//============================================================================
// Name        : main.cpp
// Author      : suchao.wang
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++
//============================================================================


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
#include <pthread.h>
#include "AwcCommon.h"
#include "Log.h"
#include "AwcModule.h"
#include "boardinfo.h"
#include "gpioinfo.h"


static char config_file[PATH_MAX];
static char project_dir[PATH_MAX];
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


sem_t sem_config;

#define APP_NAME "AdvWirelessCheck"

#ifndef VERSION_NUMBER
#define VERSION_NUMBER					""
#endif


typedef PAWC_CONNECTION (*FP_GetConnection)();

static PAWC_CONNECTION getConnection(const char *filePath)
{
	void *handle = dlopen(filePath, RTLD_NOW);
	if (!handle) {
		LOGEX("open %s failed\nERROR:%s\n", filePath , dlerror());
		return NULL;
	}

	PAWC_CONNECTION conn = NULL;
	FP_GetConnection GetConnection = (FP_GetConnection) dlsym(handle, "GetConnection");
	if (GetConnection != 0) {
		conn = GetConnection();
		LOGEX( "Load module %s, netcard %s\n", filePath, conn->ifName);
	}else
	{
		LOGEX( "Get Module %s failed\n" , filePath);
	}
	return conn;
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
	COMM_STATE_SWITCH_SIM,
};
typedef enum comm_state				COMM_STATE;

#define conn_state_switch_to(ns) \
		do \
		{ \
			comm_state = ns; \
			conn->retry_count = 0;\
			printf("switch to %d\n",ns);\
		} while ( 0 )
#define RETRYTIMES 4
static char PRE_DIR[PATH_MAX];// "/home/root/bin/"


static char serialName[1024] = { 0 };
static char sim1_ismi[16] = { 0 };
static char sim2_ismi[16]= { 0 };
static int sim_inuse = 0;

static int connProcessThread(void* arg)
{
	MODULEINFO *pinfo = (PMODULEINFO)arg;
	char mname[512];
	sprintf(mname,"%s%s",PRE_DIR,pinfo->moduleName);
	printf("%s,%s++++\n", __func__,mname);

	AWC_CONNECTION *conn;
	conn = getConnection(mname);
	if (!conn )
	{
		LOGEX("init %s failed ,quit checkConnection\n",mname);
		return -1;
	}

	conn->loadConfig(conn,pinfo);
	conn->private_data = (void *)pinfo;
	sem_post(&sem_config);

	//reinit every 5 seconds
	while(!conn->init(conn,pinfo))
	{
		LOGEX("init %s failed ,reinit in 5 seconds\n",mname);
		sleep(5);
	}

	if(pinfo->dual_sim)
	{
		strcpy(sim1_ismi,pinfo->sim1_ismi);
		strcpy(sim2_ismi,pinfo->sim2_ismi);
		sim_inuse =  pinfo->sim_inuse;
		printf("sim1:%s,%s\n",sim1_ismi,pinfo->sim1_ismi);
		printf("sim2:%s,%s\n",sim2_ismi,pinfo->sim2_ismi);
		printf("sim inuse:%d\n",pinfo->sim_inuse);
		printf("sim sim_2_change:%d\n",pinfo->sim_2_change);
		printf("sim sim_find_count:%d\n",pinfo->sim_find_count);
		printf("sim sim_master:%d\n",pinfo->sim_master);
		printf("sim sim_siwth_type:%d\n",pinfo->sim_siwth_type);
		printf("sim sim_tag_handle:%p\n",pinfo->sim_tag_handle);
	}

	if(pinfo->sem_device != NULL)
	{
		sem_post(pinfo->sem_device);
		system("killall -SIGUSR2 GPSManager");
	}


	COMM_STATE comm_state = COMM_STATE_CHECK_DEVICE;
	while (1)
	{
		switch(comm_state)
		{
		case COMM_STATE_IDLE:
			sleep(60);
			conn_state_switch_to(COMM_STATE_CHECK_REBOOT);
			break;
		case COMM_STATE_CHECK_REBOOT:
			conn_state_switch_to(COMM_STATE_CHECK_DEVICE);
			system("fixroute");
			break;
		case COMM_STATE_CHECK_DEVICE:
		{
			//restart usb if the can't find device
			if(conn->checkDevice(conn))
			{
				conn_state_switch_to(COMM_STATE_CHECK_PROCESS);
			}else{
				conn->retry_count++;
				if(conn->retry_count > RETRYTIMES )
				{
					conn_state_switch_to(COMM_STATE_RESTART_DEVICE);
				}
				else{
					LOGEX("%s,%s:checkDevice in 5 seconds[%d/%d]\n", __func__,pinfo->moduleName,conn->retry_count,RETRYTIMES);
					sleep(5);
				}
			}
		}
			break;
		case COMM_STATE_CHECK_PROCESS:
		{
			if(pinfo->sms_only)
			{
				conn_state_switch_to(COMM_STATE_IDLE);
				break;
			}
			//redial if the process is die
			if (!conn->checkProcess(conn))
			{
				LOGEX("%s,%s:checkProcess failed\n", __func__,pinfo->moduleName);
				conn_state_switch_to(COMM_STATE_REDEIAL);
			}else
				conn_state_switch_to(COMM_STATE_CHECK_CONNECTION);
		}
			break;
		case COMM_STATE_CHECK_CONNECTION:
		{
			if(pinfo->sms_only)
			{
				conn_state_switch_to(COMM_STATE_IDLE);
				break;
			}
			int ret = conn->checkConnection(conn);
//			LOGEX("%s,check connect ret=%d,recount=%d", __func__, ret, conn->retry_count);
			if( ret == CHK_CONN_SUCCESS)
			{
				conn->restart_device_count = 0;
				conn->lastConnectionTime = GetTickCount();
				conn_state_switch_to(COMM_STATE_IDLE);
			}else	if(ret == CHK_CONN_FAILED)
			{
				conn->retry_count++;
				if(conn->retry_count > RETRYTIMES )
				{
					conn_state_switch_to(COMM_STATE_REDEIAL);
				}
				else{
					LOGEX("%s,%s:checkConnection in 5 seconds[%d/%d]\n", __func__,pinfo->moduleName,conn->retry_count,RETRYTIMES);
					sleep(5);
				}

			}else{//CHK_CONN_NOT_CHECK
				conn_state_switch_to(COMM_STATE_IDLE);
			}

		}
			break;
		case COMM_STATE_REDEIAL:
		{
			int dialup=0;
			if(pinfo->sms_only)
			{
				conn_state_switch_to(COMM_STATE_IDLE);
				break;
			}
			conn_state_switch_to(COMM_STATE_RESTART_DEVICE);
			int i = 0;
			for (i = 0; i < RETRYTIMES; i++)
			{
				LOGEX("%s,%s:redialUp[%d/%d]\n", __func__,pinfo->moduleName,i+1,RETRYTIMES);
				conn->dialDown(conn);
				sleep(5);
				if (conn->dialUp(conn))
				{
					dialup = 1;
					conn_state_switch_to(COMM_STATE_IDLE);
					break;
				}
				sleep(5);
			}

			if(dialup)
			{
				conn_state_switch_to(COMM_STATE_IDLE);
				return 0;
			}

			return 1;


		}
			break;
		default:
			break;
		}

	}
	return 1;
}

static int module_load(char *config_file)
{
	int ret = 1;
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;

	int dual_sim = 0;
	SIM_MODEM *sim_modem = NULL;
	dual_sim =check_dual_sim_board(&sim_modem);
	printf("Dual  sim:%d\n",dual_sim);


	sem_init(&sem_config,0,0);

	doc = xmlParseFile(config_file);
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
					xmlChar * connect,*is4G,*sms_enable;//,*other;//,*other1,*other2;
					connect = xmlGetProp(child, BAD_CAST "connection");
					is4G = xmlGetProp(child, BAD_CAST "is4G");
					sms_enable = xmlGetProp(child, BAD_CAST "smsEnable");
					if ((connect && strcmp((char *) connect, "true") == 0) || (sms_enable && strcmp((char *) sms_enable, "true") == 0))
					{
						MODULEINFO *pinfo = (PMODULEINFO)malloc(sizeof(MODULEINFO));
						memset(pinfo,0,sizeof(MODULEINFO));

						pinfo->serial_name = serialName;

						if(connect && strcmp((char *) connect, "true") != 0)
						{
							pinfo->sms_only = 1;
						}

						if(dual_sim)
						{
							pinfo->dual_sim = 1;
							pinfo->sim_modem = sim_modem;
							pinfo->gpio_sim_index = gpio_find(sim_modem->sim_select.gpio_port,sim_modem->sim_select.gpio_index);
							pinfo->gpio_modem_index = gpio_find(sim_modem->power_pin.gpio_port,sim_modem->power_pin.gpio_index);

							printf("sim index:%d,modem index:%d\n",pinfo->gpio_sim_index,pinfo->gpio_modem_index);
						}
						if(is4G && !strcmp((char *)is4G,"true"))
							sprintf(pinfo->moduleName,"%s","awc_4g.so");
						else
							sprintf(pinfo->moduleName,"%s","awc_3g.so");
						pinfo->data = child;
						ret = connProcessThread((void*)pinfo);
					}

				}
				child = child->next;
			}
		}
		Config = Config->next;
	}
	xmlFreeDoc(doc);
	return ret;
}

int gprstest()
{
	int ret = 1;
	memset(config_file,0,sizeof(config_file));
	memset(project_dir,0,sizeof(project_dir));

	char *value = getenv("TAGLINK_PATH");
	if(value != NULL)
	{
		LOGEX("TAGLINK_PATH:%s\n",value);
		sprintf(config_file,"%s/project/SystemSetting.acr",value);
		sprintf(PRE_DIR,"%s/bin/",value);
	}else{
		sprintf(config_file,"/home/root/project/SystemSetting.acr");
		sprintf(PRE_DIR,"/home/root/bin/");
	}

	if(access(config_file,F_OK))
	{
		LOGEX("Config file: %s not exist!\n",config_file);
		return 0;
	}else{
		LOGEX("Config file: %s\n",config_file);
	}

	if(project_dir[strlen(project_dir) -1] != '/')
		project_dir[strlen(project_dir) - 1] = '/';

	gpio_init(1);
	ret = module_load(config_file);
	return ret;
}


int sim_check()
{
	int count = 60;
	while(count)
	{
		if(access("/dev/ttyUSB1",F_OK) == 0)
			break;
		sleep(1);
		count--;
	}
	int ret = get_gprs_sim_state("/dev/ttyUSB1");
	LOGEX("%s:sim state=%d\n",__func__,ret);
	return !ret;
}
