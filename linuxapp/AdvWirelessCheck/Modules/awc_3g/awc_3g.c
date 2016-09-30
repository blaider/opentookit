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
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <time.h>
#include <sys/reboot.h>
#include <limits.h>
#include <AwcModule.h>
#include <AwcCommon.h>

#define NETCARD_NAME "ppp0"
#define PROCESS_NAME "pppd"


struct awc_connection_3g
{
	struct awc_connection base;

	char processName[64];

	//for dial
	char dialUpCommand[128];

	char dialDownCommand[128];

	int debugReboot;

	//for usb
	int usbBus;

	char usbName[256];

	char usbLable[512];

	char ttyCommPort[32];

	int ttyCommPortNo;

	char ttyConfigPort[32];

	int ttyConfigPortNo;

	//for connection check
	CHECK_CONN_DATA checkConnData;

	char operator[32];

	int netmode; // 0: 2g only,2: 3G only ,1: 3G/2G auto
};


bool n3g_showConfig(AWC_CONNECTION * conn1)
{
	struct awc_connection_3g * conn = (struct awc_connection_3g *)conn1;
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
	LOGEX("%s,operator=%s",conn->base.ifName,conn->operator);
	LOGEX("%s,netmode=%d",conn->base.ifName,conn->netmode);
	return true;
}

bool n3g_init(AWC_CONNECTION * conn1)
{
	bool bret = true;
	struct awc_connection_3g * conn = (struct awc_connection_3g *)conn1;
	do{
		int i=0;
		for(i=0;i<6;i++)
		{
			if(conn1->checkDevice(conn1))
			{
				bret = true;
				break;
			}else
			{
				bret = false;
				LOGEX("%s:usb device|%s| with lable|%s| Not exist",__func__,conn->usbName,conn->usbLable);
				sleep(5);
			}
		}
		if(!bret)
		{
			LOGEX("%s:not found device,quit",__func__);
			break;
		}
		if( i > 0)
			sleep(5);
		system(conn->dialDownCommand);
		if(get_gprs_config_node_name(conn->usbLable,conn->ttyCommPortNo,conn->ttyCommPort) <=0 )
		{
			bret = false;
			LOGEX("%s:not found ttyCommPort,quit",__func__);
			break;
		}
		if(get_gprs_config_node_name(conn->usbLable,conn->ttyConfigPortNo,conn->ttyConfigPort) <= 0)
		{
			bret = false;
			LOGEX("%s:not found ttyConfigPort,quit",__func__);
			break;
		}
		if(!strcmp((char *)conn->operator,"auto"))
		{
//			generate_default_script();
//			sprintf(conn->dialUpCommand,"wan.sh default %s",conn->ttyCommPort);
//			bret = checkOperator(conn->ttyCommPort,conn->dialUpCommand);
			int ret = checkProvider(conn->ttyCommPort,conn->dialUpCommand, conn->netmode,conn->usbLable);
			LOGEX("%s:checkProvider return = %d ",__func__,bret);
			if(ret < 0)
				bret = false;
		}else{
			sprintf(conn->dialUpCommand,"wan.sh %s %s",conn->operator,conn->ttyCommPort);
		}

		break;

	}while(0);
	if(bret && !conn->usbBus)
		find_usb_bus_no(conn->usbName,&conn->usbBus);

	n3g_showConfig(conn1);
	LOGEX("return:%d\n",bret);
	return bret;
}

bool n3g_loadConfig(AWC_CONNECTION * conn1,void * data)
{
	bool bret = true;
	struct awc_connection_3g * conn = (struct awc_connection_3g *)conn1;
	xmlNodePtr child = (xmlNodePtr)data;
	if (strcmp((const char*) child->name, "GPRS") == 0 )
	{
		xmlChar * connect = NULL;//,*other;//,*other1,*other2;
		connect = xmlGetProp(child, BAD_CAST "connection");
		xmlNodePtr child1 = child->xmlChildrenNode;

		system(conn->dialDownCommand);
		if (!strcmp((char *) connect, "true"))
		{
//						conn->isEnable = true;
//						gprs_enable=1;
		}else{
//						gprs_enable=0;
//						conn->isEnable = false;

			child = child->next;
		}

		unsigned int urlIndex=0;
		while (child1 != NULL)
		{
			if (strcmp((const char*) child1->name, "Operator") == 0 )
			{
//				operator = xmlNodeGetContent(child1);
				sprintf(conn->operator,"%s",xmlNodeGetContent(child1));
			}
			if (strcmp((const char*) child1->name, "Lable") == 0 )
			{
				sprintf(conn->usbLable,"%s",xmlNodeGetContent(child1));
			}
			if (strcmp((const char*) child1->name, "NetworkMode") == 0 )
			{
				xmlChar *p = xmlNodeGetContent(child1);
				if (strcmp((const char*) p, "3G/2G") == 0 )
				{
					conn->netmode = 1;
				}else if (strcmp((const char*) p, "3G") == 0 )
				{
					conn->netmode = 2;
				}else if (strcmp((const char*) p, "2G") == 0 )
				{
					conn->netmode = 0;
				}
			}
			if (strcmp((const char*) child1->name, "CommunicationPort") == 0 )
			{
				conn->ttyCommPortNo = strtoul((char *)xmlNodeGetContent(child1),NULL,10);
			}
			if (strcmp((const char*) child1->name, "ConfigPort") == 0 )
			{
				conn->ttyConfigPortNo = strtoul((char *)xmlNodeGetContent(child1),NULL,10);
			}
			if (strcmp((const char*) child1->name, "USBDeviceName") == 0 )
			{
				sprintf(conn->usbName,"%s",xmlNodeGetContent(child1));
			}
			if (strcmp((const char*) child1->name, "PingURL") == 0 )
			{
				PINGURL *urls = &conn->checkConnData.pingurl;
				if(urlIndex < sizeof(urls->url)/sizeof(urls->url[0]))
					sprintf(urls->url[urlIndex++],"%s",xmlNodeGetContent(child1));
			}
			if (strcmp((const char*) child1->name, "ConnectionCheckType") == 0 )
			{
				xmlChar *tp = xmlNodeGetContent(child1);
				if(NULL != tp)
					conn->checkConnData.checkMode = strtoul((char *)tp,NULL,10);
			}
			if (strcmp((const char*) child1->name, "MaxSilenceTime") == 0 )
			{
				xmlChar *tp = xmlNodeGetContent(child1);
				if(NULL != tp)
					conn->checkConnData.tdetail.trafficMinutes = strtoul((char *)tp,NULL,10);
			}
			if (strcmp((const char*) child1->name, "Debug") == 0 )
			{
				xmlChar *tp = xmlNodeGetContent(child1);
				if(NULL != tp)
					conn->debugReboot = strtoul((char *)tp,NULL,10);

			}
			if (strcmp((const char*) child1->name, "RebootOnFailureTime") == 0 )
			{
				xmlChar *tp = xmlNodeGetContent(child1);
				if(NULL != tp)
				{
					int hours = strtoul((char *)tp,NULL,10);
					if(0 < hours && 24 >= hours)
						conn->base.restartSystemSeconds = hours *60*60;
				}
			}
			if (strcmp((const char*) child1->name, "PingInterval") == 0 )
			{
				xmlChar *tp = xmlNodeGetContent(child1);
				if(NULL != tp)
				{
					int seconds = strtoul((char *)tp,NULL,10);
					if(5 <= seconds &&  3600 >= seconds)
						conn->checkConnData.pingurl.pingInterval = seconds;
				}
			}
			child1 = child1->next;
		}
		if( connect != NULL)
		     xmlFree(connect);
	}

	n3g_showConfig(conn1);
	LOGEX("return:%d\n",bret);
	return bret;
}


bool n3g_dialUp(AWC_CONNECTION * conn1)
{
	struct awc_connection_3g * conn = (struct awc_connection_3g *)conn1;
	system(conn->dialUpCommand);
	return NetIsExist(conn->base.ifName);
}


bool n3g_dialDown(AWC_CONNECTION * conn1)
{
	struct awc_connection_3g * conn = (struct awc_connection_3g *)conn1;
	system(conn->dialDownCommand);
	return false;
}

bool n3g_restartDevice(AWC_CONNECTION * conn1)
{
	struct awc_connection_3g * conn = (struct awc_connection_3g *)conn1;
	restartUSB(conn->usbBus);
	sleep(40);
	if(usb_detect(conn->usbName))
	{
		return true;
	}
	return false;
}

bool n3g_checkConnection(AWC_CONNECTION * conn1)
{
	struct awc_connection_3g * conn = (struct awc_connection_3g *)conn1;
	int result=0;
		switch(conn->checkConnData.checkMode)
		{
		case 0:
			result = 1;
			break;
		case 1:
			result = system_ping(conn->base.ifName,&conn->checkConnData.pingurl);
			break;
		case 2:
			result = traffic_monitor(conn->base.ifName,&conn->checkConnData.tdetail);
			break;
		default:
			break;
		}
	return result;
}

bool n3g_checkRebootSystem(AWC_CONNECTION * conn,time_t lastTime)
{
	if(!conn->restartSystemSeconds || !lastTime)
		return true;
	 if(conn->restartSystemSeconds)
	 {
		 if(time(NULL) - lastTime > conn->restartSystemSeconds)
		 {
			return false;
		 }
	 }
	return true;
}

bool n3g_checkProcess(AWC_CONNECTION * conn1)
{
	struct awc_connection_3g * conn = (struct awc_connection_3g *)conn1;
	return checkProcess(conn->processName);
}

bool n3g_checkDevice(AWC_CONNECTION * conn1)
{
	struct awc_connection_3g * conn = (struct awc_connection_3g *)conn1;
	return usb_detect(conn->usbName) && usbLable_detect(conn->usbLable);
}

struct awc_connection_3g connection =
{
		.base = {
				.cb = sizeof(struct awc_connection),
				.ifName = NETCARD_NAME,
				.restartSystemSeconds = 0,
				.loadConfig = n3g_loadConfig,
				.init = n3g_init,
				.checkRebootSystem = n3g_checkRebootSystem,
				.checkProcess = n3g_checkProcess,
				.checkDevice = n3g_checkDevice,
				.checkConnection = n3g_checkConnection,
				.dialUp = n3g_dialUp,
				.dialDown = n3g_dialDown,
				.restartDevice = n3g_restartDevice,
				.showConfig = n3g_showConfig,
		},
		.processName = PROCESS_NAME,
		.usbBus = 0,
		.dialUpCommand = "wan.sh",
		.dialDownCommand = "/etc/ppp/ppp-off",
		.ttyCommPortNo = 0,
		.ttyConfigPortNo = 1,
		.ttyCommPort = "Not Init",
		.ttyConfigPort = "Not Init",
		.usbName = "Not Init",
		.usbLable = "Not Init",
		.netmode = 1,
};

PAWC_CONNECTION GetConnection()
{
	return (PAWC_CONNECTION)&connection;
}

