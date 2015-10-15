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
// File:   	 DataProcessMgr.cpp
// Author:  suchao.wang
// Created: Apr 29, 2015
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#include "DataProcessMgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "sharemem.h"

void* ProcessThread( void* arg )
{
	DataProcessMgr* pConnector = (DataProcessMgr*)arg;
	pConnector->Run();
	return 0;
}

uint32_t DataProcessMgr::Run()
{
	printf("%s+++++\n",__func__);
	while(sem_trywait(&m_hEvent))
	{
		printf("%s=====\n",__func__);
		PSCOM command;
		if(PopCommand(&command))
		{
			printf("%s:%p\n",__func__,command);
			command->func(this);
			free(command);
		}else
		{
			IdleStatus();
			sleep(1);
		}
	}
	printf("%s----\n",__func__);
	return 0;
}
uint32_t DataProcessMgr::Initialize()
{
	if(!init)
	{
		pthread_create(&m_hThread,NULL,ProcessThread,this);
		init = 1;
	}
	return 0;
}


#define FIFOFILE "/tmp/advprogramfifo"
void DataProcessMgr::FuncStartsProgram(DataProcessMgr *mgr)
{
	int timeout = 0;
	PSHRINFO info;
	int semvalue=0;
	ShareMemInit(&info);

	sem_getvalue(&info->sem_rw,&semvalue);
	printf("%s:1,%d\n",__func__,semvalue);
	while(sem_trywait(&info->sem_rw))
	{
		if(time(NULL) - info->status.modifytime > MAX_PROCESS_TIME)
		{
			timeout = 1;
			break;
		}

		sleep(1);
	}
	printf("%s:2,%d\n",__func__,timeout);
	if(!timeout)
	{
		info->status.status = STAT_STARTPROGRAM_START;
		info->status.modifytime = time(NULL);
	}else{
		mgr->TimeoutStatus();
	}
	printf("%s:3\n",__func__);
	sem_post(&info->sem_rw);
	sleep(2);
	while(info->status.status > STAT_STARTPROGRAM_START && info->status.status <= STAT_STARTPROGRAM_END)
	{
		printf("set new status\n");
		mgr->SetStatus(info->status);
		if(info->status.status == STAT_STARTPROGRAM_END)
		{
			break;
		}
		sleep(1);
	}
	printf("%s:4\n",__func__);

}
void DataProcessMgr::FuncStopProgram(DataProcessMgr *mgr)
{
	int timeout = 0;
	PSHRINFO info;
	int semvalue=0;
	ShareMemInit(&info);

	sem_getvalue(&info->sem_rw,&semvalue);
	printf("%s:1,%d\n",__func__,semvalue);
	while(sem_trywait(&info->sem_rw))
	{
		if(time(NULL) - info->status.modifytime > MAX_PROCESS_TIME)
		{
			timeout = 1;
			break;
		}

		sleep(1);
	}
	printf("%s:2,%d\n",__func__,timeout);
	if(!timeout)
	{
		info->status.status = STAT_STOPPROGRAM_START;
		info->status.modifytime = time(NULL);
	}else{
		mgr->TimeoutStatus();
	}
	printf("%s:3\n",__func__);
	sem_post(&info->sem_rw);
	sleep(2);
	while(info->status.status > STAT_STOPPROGRAM_START && info->status.status <= STAT_STOPPROGRAM_END)
	{
		printf("set new status\n");
		mgr->SetStatus(info->status);
		if(info->status.status == STAT_STOPPROGRAM_END)
		{
			break;
		}
		sleep(1);
	}
	printf("%s:4\n",__func__);
}
void DataProcessMgr::FuncRenameFiles(DataProcessMgr *mgr)
{
	int fileNums=0;
	char fileName[4096];
	char fileNameTemp[4096];
	int total;
//	strcpy(a,b.c_str());



	total = mgr->m_listFile.size();

	STATUS stas;
	stas.status = STAT_FILE_PROCESS;
	stas.max = total;
	stas.modifytime = time(NULL);
	stas.index = 0;
	mgr->SetStatus(stas);


	list<string>::iterator itor;
	for (itor = mgr->m_listFile.begin(); itor != mgr->m_listFile.end(); itor++)  // 遍历list
	{
		string &str= (*itor);
		strcpy(fileName,str.c_str());
		strcpy(fileNameTemp,fileName);
		strcat(fileNameTemp,".tmp");
		if(!access(fileName,F_OK))
			remove(fileName);
		if(!access(fileNameTemp,F_OK))
			rename(fileNameTemp,fileName);
		sync();
		fileNums++;
		printf("%d/%d %s\n",fileNums,total,fileName);
		stas.index = fileNums;
		stas.modifytime =  time(NULL);
		mgr->SetStatus(stas);
		sleep(1);
	}


	mgr->m_listFile.clear();
	stas.status = STAT_FILE_END;
	stas.modifytime = time(NULL);
	mgr->SetStatus(stas);
	return ;
}

uint32_t DataProcessMgr::StopProgram()
{
	PSCOM command = (PSCOM)malloc(sizeof(SCOM));
	if(NULL == command)
		return 0;
	command->func = FuncStopProgram;
	PushCommand(command);
	printf("%s:%p\n",__func__,command);
	return 0;
}
uint32_t DataProcessMgr::StartProgram()
{
	PSCOM command = (PSCOM)malloc(sizeof(SCOM));
	if(NULL == command)
		return 0;
	command->func = FuncStartsProgram;
	PushCommand(command);
	printf("%s:%p\n",__func__,command);
	return 1;
}

uint32_t DataProcessMgr::RenameFiles(list<string> &list)
{
	m_listFile = list;
	PSCOM command = (PSCOM)malloc(sizeof(SCOM));
	if(NULL == command)
		return 0;
	command->func = FuncRenameFiles;
	PushCommand(command);
	printf("%s:%p\n",__func__,command);
	return 1;
}

STATUS DataProcessMgr::GetStatus()
{
	STATUS status;
	pthread_mutex_lock(&m_StatusLock);
	status = m_Status;
	pthread_mutex_unlock(&m_StatusLock);
	return status;
}
uint32_t DataProcessMgr::IdleStatus()
{
	if(STAT_IDLE == m_Status.status)
		return 0;
	STATUS status;
	status.status = STAT_IDLE;
	status.index = 0;
	status.max = 0;
	status.modifytime = time(NULL);
	pthread_mutex_lock(&m_StatusLock);
	m_Status = status;
	pthread_mutex_unlock(&m_StatusLock);
	return 0;
}
uint32_t DataProcessMgr::TimeoutStatus()
{
	if(STAT_TIMEOUT == m_Status.status)
		return 0;
	STATUS status;
	status.status = STAT_TIMEOUT;
	status.index = 0;
	status.max = 0;
	status.modifytime = time(NULL);
	pthread_mutex_lock(&m_StatusLock);
	m_Status = status;
	pthread_mutex_unlock(&m_StatusLock);
	return 0;
}
uint32_t DataProcessMgr::SetStatus(STATUS status)
{
	pthread_mutex_lock(&m_StatusLock);
	status.modifytime =  time(NULL);
	m_Status = status;
	pthread_mutex_unlock(&m_StatusLock);
	return 0;
}

uint32_t DataProcessMgr::PopCommand(PSCOM *command)
{
	uint32_t ret=0;
	pthread_mutex_lock(&m_CommandLock);
//	printf("%s:%d\n",__func__,m_listCommand.size());
	if(m_listCommand.size() > 0)
	{
		*command = m_listCommand.front();
		m_listCommand.pop_front();
		ret = 1;
//		printf("%s:%p\n",__func__,*command);
	}
//	printf("%s:%d\n",__func__,m_listCommand.size());
	pthread_mutex_unlock(&m_CommandLock);
	return ret;

}
uint32_t DataProcessMgr::PushCommand(PSCOM command)
{
	pthread_mutex_lock(&m_CommandLock);
	m_listCommand.push_back(command);
//	printf("%s:%p,%d\n",__func__,command,m_listCommand.size());
	pthread_mutex_unlock(&m_CommandLock);
	return 0;
}


