// #############################################################################
// *****************************************************************************
//                  Copyright (c) 2011, Advantech Automation Corp.
//      THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY
//               INFORMATION WHICH IS THE PROPERTY OF ADVANTECH AUTOMATION CORP.
//
//    ANY DISCLOSURE, USE, OR REPRODUCTION, WITHOUT WRITTEN AUTHORIZATION FROM
//               ADVANTECH AUTOMATION CORP., IS STRICTLY PROHIBITED.
// *****************************************************************************
// #############################################################################
//
// File:			Log.h
// Author:			Chen.Hao
// Created:			17/2/2011   9:17
//
// Description:		Program running log file class.
// -----------------------------------------------------------------------------

#ifndef _LOG_H
#define _LOG_H
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#include <syslog.h>

void Log(const char* wzMsg );
void LogFormat(const char * wzMsgFormat, ... );
void LogBuffer(const char * name,const unsigned char* buffer, int length );


#define LOG(x)			Log(x)
#define LOGEX			LogFormat
#define LOGBUF			LogBuffer

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
