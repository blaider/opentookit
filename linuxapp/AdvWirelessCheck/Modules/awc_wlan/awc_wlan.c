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
// File:   	 acw_wlan.c
// Author:  suchao.wang
// Created: Jun 25, 2015
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <time.h>

#include <AwcCommon.h>
#include <AwcModule.h>


#define NETCARD_NAME "wlan0"
#define PROCESS_NAME "wpa_supplicant"


struct awc_connection_wlan
{
	struct awc_connection base;

	char processName[64];

	time_t lastConnTime;

	bool isEnable;


	int checkNetConnectionMode;

	int usbBus;

	//for dial
	char dialUpCommand[128];

	char dialDownCommand[128];

	//for connection check
	TRAFFICDETAIL tdetail;

	int pingInterval;

	char pingUrl[4][1024];

	int trafficMinutes;

	int debugReboot;

	//for usb
	char usbName[256];

	char usbLable[512];

	char ttyCommPort[32];

	int ttyCommPortNo;

	char ttyConfigPort[32];

	int ttyConfigPortNo;
};

bool wlan_showConfig(AWC_CONNECTION * conn1)
{
	struct awc_connection_wlan * conn = (struct awc_connection_wlan *)conn1;
	LOGEX("%s,cb=%d",conn->base.ifName,conn->base.cb);
	LOGEX("%s,restartSystemSeconds=%d",conn->base.ifName,conn->base.restartSystemSeconds);
	LOGEX("%s,dialUpCommand=%s",conn->base.ifName,conn->dialUpCommand);
	LOGEX("%s,dialDownCommand=%s",conn->base.ifName,conn->dialDownCommand);
	LOGEX("%s,processName=%s",conn->base.ifName,conn->processName);
	LOGEX("%s,ttyCommPort=%s",conn->base.ifName,conn->ttyCommPort);
	LOGEX("%s,ttyConfigPort=%s",conn->base.ifName,conn->ttyConfigPort);
	LOGEX("%s,ttyCommPortNo=%d",conn->base.ifName,conn->ttyCommPortNo);
	LOGEX("%s,ttyConfigPortNo=%d",conn->base.ifName,conn->ttyConfigPortNo);
	LOGEX("%s,usbName=%s",conn->base.ifName,conn->usbName);
	LOGEX("%s,usbLable=%s",conn->base.ifName,conn->usbLable);
	LOGEX("%s,usbBus=%d",conn->base.ifName,conn->usbBus);
	return true;
}

bool wlan_loadConfig(AWC_CONNECTION * conn1,void * data)
{
	struct awc_connection_wlan * conn = (struct awc_connection_wlan *)conn1;
	xmlNodePtr child =(xmlNodePtr)data ;
	while (child != NULL)
	{
		if (strcmp((const char*) child->name, "WiFi") == 0)
		{
			xmlChar *enables;
			enables = xmlGetProp(child, BAD_CAST "Enable");
			if (!strcmp((char *) enables, "true"))
			{
				conn->isEnable = true;
			}else{
				conn->isEnable = false;
				if(!access("/etc/wpa_supplicant.conf",F_OK))
				{
					remove("/etc/wpa_supplicant.conf");
				}
			}
		}
		child = child->next;
	}
	wlan_showConfig(conn1);
	return true;
}

bool wlan_init(AWC_CONNECTION * conn1)
{
//	struct awc_connection_wlan * conn = (struct awc_connection_wlan *)conn1;
//	wlan_showConfig(conn1);
	return true;
}

bool wlan_dialUp(AWC_CONNECTION * conn1)
{
	struct awc_connection_wlan * conn = (struct awc_connection_wlan *)conn1;
	system(conn->dialUpCommand);
	return NetIsExist(conn->base.ifName) && checkProcess(conn->processName);
}

bool wlan_dialDown(AWC_CONNECTION * conn1)
{
	struct awc_connection_wlan * conn = (struct awc_connection_wlan *)conn1;
	system(conn->dialDownCommand);
	return true;
}

bool wlan_restartModem(AWC_CONNECTION * conn1)
{
	struct awc_connection_wlan * conn = (struct awc_connection_wlan *)conn1;
	int i = 0;
	for (i = 0; i < RETRYTIMES; i++)
	{
		restartUSB(conn->usbBus);
		sleep(40);
		if(usb_detect(conn->usbName))
		{
			break;
		}
	}
	return false;
}


bool wlan_checkConnection(AWC_CONNECTION * conn)
{
//	wlan_loadConfig(conn);
	return true;
}

bool wlan_checkDaemonProcess(AWC_CONNECTION * conn1)
{
	struct awc_connection_wlan * conn = (struct awc_connection_wlan *)conn1;
	return checkProcess(conn->processName);
}

bool wlan_checkDeviceExist(AWC_CONNECTION * conn1)
{
	struct awc_connection_wlan * conn = (struct awc_connection_wlan *)conn1;
	return NetIsExist(conn->base.ifName);
}
bool wlan_checkRebootSystem(AWC_CONNECTION * conn1,time_t lastTime)
{
	return true;
}

struct awc_connection_wlan connection =
{
		.base = {
				.cb = sizeof(struct awc_connection),
				.ifName = NETCARD_NAME,
				.restartSystemSeconds = 0,
				.init = wlan_init,
				.loadConfig = wlan_loadConfig,
				.checkRebootSystem = wlan_checkRebootSystem,
				.checkProcess = wlan_checkDaemonProcess,
				.checkDevice = wlan_checkDeviceExist,
				.checkConnection = wlan_checkConnection,
				.dialUp = wlan_dialUp,
				.dialDown = wlan_dialDown,
				.restartDevice = wlan_restartModem,
				.showConfig = wlan_restartModem,
		},
		.processName = PROCESS_NAME,
		.checkNetConnectionMode = 0,
		.usbBus = 0,
		.isEnable = 0,
		.dialUpCommand = "wlan.sh up",
		.dialDownCommand = "wlan.sh down",

};

PAWC_CONNECTION GetConnection()
{
	return (PAWC_CONNECTION)&connection;
}

