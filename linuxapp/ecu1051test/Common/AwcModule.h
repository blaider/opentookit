// #############################################################################
// *****************************************************************************
//                  Copyright (c) 2015, Advantech Automation Corp.
//      THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY
//               INFORMATION WHICH IS THE PROPERTY OF ADVANTECH AUTOMATION CORP.
//
//    ANY DISCLOSURE, USE, OR REPRODUCTION, WITHOUT WRITTEN AUTHORIZATION FROM
//               ADVANTECH AUTOMATION CORP., IS STRICTLY PROHIBITED.
// *****************************************************************************
// #############################################################################
//
// File:   	 AwcModule.h
// Author:  suchao.wang
// Created: Jun 25, 2015
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#ifndef AWCMODULE_H_
#define AWCMODULE_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>
#include <dlfcn.h>
#include <time.h>
#include <semaphore.h>
//#include "../Log.h"
#include <odm_dual_sim_board.h>

#define RETRYTIMES 4

typedef struct awc_connection AWC_CONNECTION,*PAWC_CONNECTION;

typedef struct _traffic_detail
{
	time_t mtime;
	unsigned int rxbytes;
	unsigned int txbytes;
	unsigned int tobytes;
	unsigned int trafficMinutes;
}TRAFFICDETAIL,*PTRAFFICDETAIL;
typedef struct _pingUrl
{
	time_t lastPingTime;
	int pingInterval;
	char url[4][256];
}PINGURL;

typedef struct _s_check_conn
{
	int checkMode;
	TRAFFICDETAIL tdetail;
	PINGURL pingurl;
}CHECK_CONN_DATA;


enum{
	CHK_MODE_NO_CHECK,
	CHK_MODE_PING,
	CHK_MODE_TRAFFIC,
};

enum{
	CHK_CONN_FAILED,
	CHK_CONN_SUCCESS,
	CHK_CONN_NOT_CHECK,
};

struct awc_connection
{
	int cb;

	char ifName[7];

	int restartSystemSeconds;

	bool (*loadConfig)(AWC_CONNECTION * conn,void *data);

	bool (*init)(AWC_CONNECTION * conn,void *data);

	bool (*checkRebootSystem)(AWC_CONNECTION * conn,time_t lastTime);

	bool (*checkProcess)(AWC_CONNECTION * conn);

	bool (*checkDevice)(AWC_CONNECTION * conn);

	int (*checkConnection)(AWC_CONNECTION * conn);

	bool (*dialUp)(AWC_CONNECTION * conn);

	bool (*dialDown)(AWC_CONNECTION * conn);

	bool (*restartDevice)(AWC_CONNECTION * conn);

	bool (*showConfig)(AWC_CONNECTION * conn);

	bool (*switch_sim)(AWC_CONNECTION * conn,void *data);

	int retry_count;

	int restart_device_count;

	time_t lastConnectionTime;

	void *private_data;
};

PAWC_CONNECTION GetConnection();

enum {
	SIM_ATUO = 0,
	SIM_DCTAG = 1,
	SIM_CONNECTON_CHECK = 2,
};

typedef struct _module_info
{
	char moduleName[32];
	void *data;
	sem_t *sem_device;
	char *serial_name;
	int dual_sim;// 0 signal sime,1 dual sim;
	int sim_find_count;  //sim card find
	int sim_inuse; // 0 no sim,1 for sim1,2 form sim2
	int sim_2_change;//0 no set,1 for sim1,2 for sim2
	char sim1_ismi[16];
	char sim2_ismi[16];
	int gpio_sim_index;
	int gpio_modem_index;
	int sim_siwth_type; // 0:auto,1:sim1,2:sim2;
	void *sim_tag_handle;
	char sim_tag_name[256];
	int sim_master;// 0:auto,1:sim1,2:sim2;
	SIM_MODEM *sim_modem;
	int sms_only;

}MODULEINFO,*PMODULEINFO;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AWCMODULE_H_ */
