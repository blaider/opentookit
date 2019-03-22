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
#include <unistd.h>
#include <time.h>
#include <net/if_arp.h>		/* For ARPHRD_ETHER */
#include <sys/socket.h>		/* For AF_INET & struct sockaddr */
#include <netinet/in.h>         /* For struct sockaddr_in */
#include <netinet/if_ether.h>
#include <arpa/inet.h>

struct in_addr GetIP( char const * if_name );

int NetIsExist(const char *ifname);

int restartUSB(int usbBusNo);

int usbLable_detect(char *usbLable);

int loadOperator(int code,char *buf);

int find_usb_bus_no (const char *usbID, int *usbBusNo);

int checkOperator(char *ttyPort,char *command);

int checkProvider ( char *ttyPort, char *command, int netmode,char *usbLabel);


int check_cdma(char * ttyName);

int get_gprs_config_node_name(char *usbLable,int ttyPortNo,char *ttyPort);

int get_gprs_sim_state(char * ttyName);

int get_mobile_csq(char * ttyName,int *csq);

int checkProcess(const char *name);

int system_ping(const char *ifname,PINGURL * urls);

int traffic_monitor(const char *ifName,TRAFFICDETAIL *tdetail);

bool find_provider_apn (
	char *apnvalue,
	size_t len,
	char const * mcc,
	char const * mnc );

int generate_default_script(int mode, char *usbLabel);

int generate_apn_netmode_script(char *apnname , int mode, char *usbLabel);

int generate_cdma_script();

unsigned long GetTickCount();

char *strlwr(char *str);

int get_system_ip(const char *ifname,in_addr_t *add);

int net_is_up(char* net_name);


struct _wifi_device_list{
	struct _wifi_device_list *next;
	char display_name[64];
	char device_id[12];
};

typedef struct _wifi_device_list WIFI_DEV_LIST;

int find_usb_bus_by_list (WIFI_DEV_LIST *list,char *usbid);
int load_usb_device_list (char *configfile,WIFI_DEV_LIST **list);

int find_usbdevice_by_label(const char *lable);
int find_usbdevice_by_id(const char *devicid);



struct _shminfo
{
	int gprs;
	int wifi;
};
typedef struct _shminfo SHMINFO;

SHMINFO * ShareMemInit();

int get_mobile_ismi(char * ttyName,char *ismi);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* NETDEVICEINFO_H_ */
