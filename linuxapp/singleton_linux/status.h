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
// File:   	 status.h
// Author:  suchao.wang
// Created: May 6, 2015
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////


#ifndef STATUS_H_
#define STATUS_H_
#include <stdint.h>
#include <time.h>

enum status
{
	STAT_IDLE = 0,
	STAT_TIMEOUT = 1,
	STAT_STOPPROGRAM_START = 10,
	STAT_STOPPROGRAM_PROCESS,
	STAT_STOPPROGRAM_END,
	STAT_FILE_START = 13,
	STAT_FILE_PROCESS,
	STAT_FILE_END,
	STAT_RENAME_START = 16,
	STAT_RENAME_PROCESS,
	STAT_RENAME_END,
	STAT_STARTPROGRAM_START = 19,
	STAT_STARTPROGRAM_PROCESS,
	STAT_STARTPROGRAM_END,
};

typedef struct _status
{
	uint32_t status;
	uint32_t index;
	uint32_t max;
	uint32_t seconds;
	time_t modifytime;
}STATUS,*PSTATUS;

#endif /* STATUS_H_ */
