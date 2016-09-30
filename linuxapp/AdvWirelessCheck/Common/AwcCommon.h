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
// File:   	 NetdeviceInfo.h
// Author:  suchao.wang
// Created: Jul 18, 2014
//
// Description:  get or set info for netdevice.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#ifndef AWCCOMMON_H_
#define AWCCOMMON_H_
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "AwcModule.h"
#include <netinet/in.h>

struct in_addr GetIP( char const * if_name );

int NetIsExist(const char *ifname);

int restartUSB(int usbBusNo);

int usb_detect (const char *usbName);

int usbLable_detect(char *usbLable);

int loadOperator(int code,char *buf);

int find_usb_bus_no (const char *usbName,int *usbBusNo);

int checkOperator(char *ttyPort,char *command);

int checkProvider ( char *ttyPort, char *command, int netmode,char *usbLabel);

int generate_apn_netmode_script(char *apnname , int mode, char *usbLabel);

int check_cdma(char * ttyName);

int get_gprs_config_node_name(char *usbLable,int ttyPortNo,char *ttyPort);

int checkProcess(const char *name);

int system_ping(const char *ifname,PINGURL * urls);

int traffic_monitor(const char *ifName,TRAFFICDETAIL *tdetail);

bool find_provider_apn (
	char *apnvalue,
	size_t len,
	char const * mcc,
	char const * mnc );

int generate_default_script();

int generate_apn_script(char *apnname);

int generate_cdma_script();

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* NETDEVICEINFO_H_ */
