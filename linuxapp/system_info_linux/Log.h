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

#pragma once
#ifdef WIN32
#include <WTypes.h>
#include <string>
#else
#include <string>
#define wchar_t char
#define L
#include <syslog.h>
#endif


using namespace std;

// export/import definition

#define MAX_LOG_LENGTH 1024

// *****************************************************************************
// Design Notes:  log class
// -----------------------------------------------------------------------------
class CALog
{
public:
	~CALog(void);

	static CALog* Instance();

	// -------------------------------------------------------------------------
	// Design Notes:  write log
	//		
	// Parameters:
	//		wzMsg: log message;
	// Returns:
	//		void.
	// -------------------------------------------------------------------------
	void Log(const wchar_t* wzMsg );

	// -------------------------------------------------------------------------
	// Design Notes:  write log
	//		
	// Parameters:
	//		wzMsgFormat: log message format;
	// Returns:
	//		void.
	// -------------------------------------------------------------------------
	void LogFormat(const wchar_t* wzMsgFormat, ... );

	// -------------------------------------------------------------------------
	// Design Notes:  write log
	//		
	// Parameters:
	//		wzMsgFormat: log message format;
	// Returns:
	//		void.
	// -------------------------------------------------------------------------
	void LogBuffer(const char * name,const unsigned char * buffer, int length );

private:

	CALog(void);

	// -------------------------------------------------------------------------
	// Design Notes:  get log file path.
	// Parameters:
	//		pwzLogPath: log path.
	// Returns:
	//		LPWSTR: log file path.
	// -------------------------------------------------------------------------
	void GetLogPath( wchar_t* pwzLogPath );
	
	// -------------------------------------------------------------------------
	// Design Notes:  log file handle.
	// -------------------------------------------------------------------------
#ifdef WIN32
	HANDLE m_hFile; 
#else
	FILE * m_hFile;
#endif
	// -------------------------------------------------------------------------
	// Design Notes:  log file path.
	// -------------------------------------------------------------------------
#ifdef WIN32
	wstring m_wzLogFilePath;
#else
	char * m_wzLogFilePath;
#endif
};

#define LOG(x)			CALog::Instance()->Log(x)
#define LOGEX			CALog::Instance()->LogFormat
#define LOGBUF			CALog::Instance()->LogBuffer
//#define LOG(x)
//#define LOGEX
//#define LOGBUF

#ifdef _DEBUG

#define DEBUGLOG(x)		CALog::Instance()->Log(x)
#define DEBUGLOGEX		CALog::Instance()->LogFormat

#else

#define DEBUGLOG(x)		{}
#define DEBUGLOGEX		{}

#endif
