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
// File:			Log.cpp
// Author:			Chen.Hao
// Created:			17/2/2011   9:17
//
// Description:		Program running log file class.
// -----------------------------------------------------------------------------
#include <limits.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include "Log.h"
#include "stdio.h"

// -------------------------------------------------------------------------
// Design Notes:  write log
//		
// Parameters:
//		wzMsg: log message;
// Returns:
//		void.
// -------------------------------------------------------------------------
void Log(const char * wzMsg )
{
	syslog(LOG_INFO, "%s", wzMsg);

}

// -----------------------------------------------------------------------------
// Design Notes:  write log
//		
// Parameters:
//		wzMsgFormat: log message format;
// Returns:
//		void.
// -----------------------------------------------------------------------------
void LogFormat(const char * wzMsgFormat, ... )
{
	char wzLog[4096] = {0};

	va_list args;
	va_start(args, wzMsgFormat);
	vsprintf( wzLog ,wzMsgFormat,args);
	va_end(args);
	syslog(LOG_INFO,"%s", wzLog);
	printf("%s",wzLog);
}

// -----------------------------------------------------------------------------
// Design Notes:  write log
//		
// Parameters:
//		wzMsgFormat: log message format;
// Returns:
//		void.
// -----------------------------------------------------------------------------
void LogBuffer(const char * name,const unsigned char* buffer, int length )
{
	char szLog[4096] = {0};
	int offset = 0;

	offset += sprintf( szLog,
			"%s,len:%3d,",
			name,
			length);

	int i= 0;
	while (i < length)
	{
		offset += sprintf(szLog + offset,"%02x ",*(buffer+i));
		i++;
	}
	syslog(LOG_INFO, "%s", szLog);
	printf("%s",szLog);
}

