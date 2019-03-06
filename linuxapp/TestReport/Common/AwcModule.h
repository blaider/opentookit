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
#include "Log.h"

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

struct awc_connection
{
	int cb;

	char ifName[7];

	int restartSystemSeconds;

	bool (*loadConfig)(AWC_CONNECTION * conn,void *data);

	bool (*init)(AWC_CONNECTION * conn);

	bool (*checkRebootSystem)(AWC_CONNECTION * conn,time_t lastTime);

	bool (*checkProcess)(AWC_CONNECTION * conn);

	bool (*checkDevice)(AWC_CONNECTION * conn);

	bool (*checkConnection)(AWC_CONNECTION * conn);

	bool (*dialUp)(AWC_CONNECTION * conn);

	bool (*dialDown)(AWC_CONNECTION * conn);

	bool (*restartDevice)(AWC_CONNECTION * conn);

	bool (*showConfig)(AWC_CONNECTION * conn);
};

PAWC_CONNECTION GetConnection();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AWCMODULE_H_ */
