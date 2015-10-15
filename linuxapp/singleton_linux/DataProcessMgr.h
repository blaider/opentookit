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
// File:   	 DataProcessMgr.h
// Author:  suchao.wang
// Created: Apr 29, 2015
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#ifndef DATAPROCESSMGR_H_
#define DATAPROCESSMGR_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstddef>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <list>

#include "status.h"

using namespace std;

class DataProcessMgr;

typedef struct _Command
{
	uint32_t command;
	void (*func)(DataProcessMgr *mgr);
}SCOM,*PSCOM;

class DataProcessMgr
{
private:
	DataProcessMgr()
	{
		m_Status.status = STAT_IDLE;
		m_Status.max = 0;
		m_Status.index = 0;
		m_Status.modifytime =  time(NULL);
		sem_init(&m_hEvent,0,0);
		m_hThread  = 0;
		pthread_mutex_init(&m_StatusLock,NULL);
		pthread_mutex_init(&m_CommandLock,NULL);
		init = 0;
	}
	virtual ~DataProcessMgr()
	{

	}
	static DataProcessMgr *m_pInstance;
	uint32_t 	init;
	STATUS	m_Status;
	class CGarbo   //它的唯一工作就是在析构函数中删除CSingleton的实例
	{
	public:
		~CGarbo()
		{
			if(DataProcessMgr::m_pInstance)
			delete DataProcessMgr::m_pInstance;
		}
	};
	static CGarbo Garbo;  //定义一个静态成员变量，程序结束时，系统会自动调用它的析构函数
	// -------------------------------------------------------------------------
	// task thread handle
	// -------------------------------------------------------------------------
	pthread_t	m_hThread;

	// -------------------------------------------------------------------------
	// task event handle
	// -------------------------------------------------------------------------
	sem_t m_hEvent;
	pthread_mutex_t m_StatusLock;
	pthread_mutex_t m_CommandLock;
	list<PSCOM>	m_listCommand;
protected:
	list<string> m_listFile;
public:
	// Description: singleton
	static DataProcessMgr* GetInstance()
	{
		static DataProcessMgr instance;
		return &instance;
	}

	uint32_t Run();
	uint32_t IdleStatus();
	uint32_t TimeoutStatus();
	STATUS GetStatus();
	uint32_t SetStatus(STATUS status);
	uint32_t StopProgram();
	uint32_t StartProgram();
	uint32_t PopCommand(PSCOM *command);
	uint32_t PushCommand(PSCOM command);
	uint32_t Initialize();
	uint32_t RenameFiles(list<string> &list);
	static void FuncStartsProgram(DataProcessMgr * mgr);
	static void FuncStopProgram(DataProcessMgr * mgr);
	static void FuncRenameFiles(DataProcessMgr * mgr);
};

#endif /* DATAPROCESSMGR_H_ */
