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
// File:   	 systeminfo.h
// Author:  suchao.wang
// Created: Oct 27, 2014
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////


#ifndef SYSTEMINFO_H_
#define SYSTEMINFO_H_

#ifdef __cplusplus
extern "C" {
#endif
int sysinfo_init();
//system info
int get_system_cpu_freq( double *freq );
int get_system_cpu_used( double *used );
int get_system_memory_size( double *total );
int get_system_memory_used( double *used );
int get_system_uptime(double *time);
int get_system_utctime(double *time);
int get_system_tf_capacity(double *capacity);
int get_system_tf_free(double *free);
int get_system_sd_capacity(double *capacity);
int get_system_sd_free(double *free);
int get_system_node_id(double *nodeid);
int get_system_serial_count(double *count);
int get_system_lan_count(double *count);
//icdm
int get_icdm_serial_mode(double *count,int no);
int get_icdm_serial_score(double *count,int no);
int get_icdm_lan_linkstatus(double *count,int no);
int get_icdm_lan_score(double *count,int no);

#ifdef __cplusplus
}
#endif
#endif /* SYSTEMINFO_H_ */
