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

int find_usb_bus_no (const char *usbName,int *usbBusNo);

int checkProcess(const char *name);

int system_ping(const char *ifname,PINGURL * urls);

int get_program_path(char *path);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* NETDEVICEINFO_H_ */
