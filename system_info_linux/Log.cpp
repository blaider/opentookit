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
#ifndef WIN32
#include <limits.h>
#include <cstddef>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#endif
#include "Log.h"
#include "stdio.h"

CALog::CALog(void)
{
#ifdef WIN32
	TCHAR wzPath[MAX_PATH] = {0};
	GetLogPath( wzPath );
	m_wzLogFilePath = wzPath;
	m_hFile = CreateFile( wzPath,									// file to open
						  GENERIC_READ | GENERIC_WRITE,				// open for writing
						  FILE_SHARE_READ | FILE_SHARE_WRITE,		// share for reading and writing
						  NULL,										// default security
						  OPEN_ALWAYS,								// existing file only
						  FILE_ATTRIBUTE_NORMAL,					// normal file
						  NULL );									// no attr. template
#else

#endif
}

CALog::~CALog(void)
{
#ifdef WIN32
	CloseHandle( m_hFile );
#else

#endif

}

CALog* CALog::Instance()
{
	static CALog instantce;
	return &instantce;
}

// -------------------------------------------------------------------------
// Design Notes:  get log file path.
// Parameters:
//		pwzLogPath: log path.
// Returns:
//		LPWSTR: log file path.
// -------------------------------------------------------------------------
void CALog::GetLogPath( wchar_t* pwzLogPath )
{
#ifdef WIN32
	if ( pwzLogPath == NULL )
	{
		return;
	}

	TCHAR wzModule[MAX_PATH]; 

	int nlen = GetModuleFileName( NULL, wzModule, MAX_PATH ) ;
	while ( wzModule[nlen-1] != '\\' )
	{
		nlen-- ;
	}
	wzModule[nlen] = 0;

	wcscpy_s( (wchar_t*)pwzLogPath, MAX_PATH, wzModule );
	wcscat_s( (wchar_t*)pwzLogPath, MAX_PATH, L"AdvAgent.LOG" );
#else
	char szPath[PATH_MAX];
	int i;
	size_t rslt = readlink("/proc/self/exe", szPath, sizeof(szPath) - 1);
	if (rslt < 0 || (rslt >= sizeof(szPath) - 1)) {
		return;
	}
	szPath[rslt] = '\0';
	for (i = rslt; i >= 0; i--) {
		if (szPath[i] == '/') {
			szPath[i + 1] = '\0';
			break;
		}
	}
	strcpy(pwzLogPath, szPath);
	strcat(pwzLogPath, "AdvAgent.LOG");
#endif
}

#ifndef WIN32
unsigned long get_file_size(const char *path)
{
	unsigned long filesize = -1;
	struct stat statbuff;
	if(stat(path, &statbuff) < 0){
		return filesize;
	}else{
		filesize = statbuff.st_size;
	}
	return filesize;
}
#endif
// -------------------------------------------------------------------------
// Design Notes:  write log
//		
// Parameters:
//		wzMsg: log message;
// Returns:
//		void.
// -------------------------------------------------------------------------
void CALog::Log(const wchar_t* wzMsg )
{
#ifdef WIN32
	// Maximal file size is 10M.
	// If it is exceed it, delete log file, and create an new empty file.
	DWORD dwFileSize = GetFileSize( m_hFile, NULL );
	if ( dwFileSize > 10 * 1024 * 1024 )
	{
		CloseHandle( m_hFile );
		DeleteFile( m_wzLogFilePath.c_str() );

		m_hFile = CreateFile( m_wzLogFilePath.c_str(),					// file to open
							  GENERIC_READ | GENERIC_WRITE,				// open for writing
							  FILE_SHARE_READ | FILE_SHARE_WRITE,		// share for reading and writing
							  NULL,										// default security
							  OPEN_ALWAYS,								// existing file only
							  FILE_ATTRIBUTE_NORMAL,					// normal file
							  NULL );									// no attr. template
	}

	WCHAR wzLog[MAX_LOG_LENGTH] = {0};
	SYSTEMTIME st;

	GetLocalTime(&st);
	wsprintf( wzLog, 
			  L"%4d-%d-%d %2d:%2d:%2d.%3d: ",
			  st.wYear,
			  st.wMonth,
			  st.wDay,
			  st.wHour,
			  st.wMinute,
			  st.wSecond,
			  st.wMilliseconds );

	wcscat_s( wzLog, MAX_LOG_LENGTH, wzMsg );

	DWORD dwNum = WideCharToMultiByte( CP_ACP, NULL, wzLog, -1, NULL, 0, NULL, FALSE );
	char *szLog;
	szLog = new char[dwNum+1];
	ZeroMemory( szLog, dwNum+1 );
	if( szLog != NULL )
	{
		WideCharToMultiByte( CP_ACP, NULL, wzLog, -1, szLog, dwNum, NULL, FALSE );
	}

	szLog[dwNum-1] = '\r';
	szLog[dwNum] = '\n';

	DWORD dwNumberToWrite = dwNum+1;
	DWORD dwNumberWrite = 0;
	DWORD dwPos = SetFilePointer( m_hFile, 0, NULL, FILE_END );
	BOOL bRet = WriteFile( m_hFile, szLog, dwNumberToWrite, &dwNumberWrite, NULL );

	delete []szLog;
#else
	syslog(LOG_INFO,wzMsg);

#endif
}

// -----------------------------------------------------------------------------
// Design Notes:  write log
//		
// Parameters:
//		wzMsgFormat: log message format;
// Returns:
//		void.
// -----------------------------------------------------------------------------
void CALog::LogFormat(const wchar_t* wzMsgFormat, ... )
{
#ifdef WIN32
	// Maximal file size is 10M.
	// If it is exceed it, delete log file, and create an new empty file.
	DWORD dwFileSize = GetFileSize( m_hFile, NULL );
	if ( dwFileSize > 10 * 1024 * 1024 )
	{
		CloseHandle( m_hFile );
		DeleteFile( m_wzLogFilePath.c_str() );

		m_hFile = CreateFile( m_wzLogFilePath.c_str(),					// file to open
							  GENERIC_READ | GENERIC_WRITE,				// open for writing
							  FILE_SHARE_READ | FILE_SHARE_WRITE,		// share for reading and writing
							  NULL,										// default security
							  OPEN_ALWAYS,								// existing file only
							  FILE_ATTRIBUTE_NORMAL,					// normal file
							  NULL );									// no attr. template
	}

	WCHAR wzLog[MAX_LOG_LENGTH] = {0};
	SYSTEMTIME st;

	GetLocalTime(&st);
	wsprintf( wzLog, 
			  L"%4d-%d-%d %2d:%2d:%2d.%3d: ", 
			  st.wYear,
			  st.wMonth,
			  st.wDay,
			  st.wHour,
			  st.wMinute,
			  st.wSecond,
			  st.wMilliseconds );

	size_t Len = wcslen( wzLog );
	va_list args;
	va_start(args, wzMsgFormat);
	_vsnwprintf_s( wzLog+Len , MAX_LOG_LENGTH-Len-3, _TRUNCATE, wzMsgFormat, args );
	va_end(args);

	DWORD dwNum = WideCharToMultiByte( CP_ACP, NULL, wzLog, -1, NULL, 0, NULL, FALSE );
	char *szLog;
	szLog = new char[dwNum+1];
	ZeroMemory( szLog, dwNum+1 );
	if( szLog != NULL )
	{
		WideCharToMultiByte( CP_ACP, NULL, wzLog, -1, szLog, dwNum, NULL, FALSE );
	}

	szLog[dwNum-1] = '\r';
	szLog[dwNum] = '\n';

	DWORD dwNumberToWrite = dwNum+1;
	DWORD dwNumberWrite = 0;
	DWORD dwPos = SetFilePointer( m_hFile, 0, NULL, FILE_END );
	BOOL bRet = WriteFile( m_hFile, szLog, dwNumberToWrite, &dwNumberWrite, NULL );

	delete []szLog;
#else
	char wzLog[MAX_LOG_LENGTH] = {0};

	va_list args;
	va_start(args, wzMsgFormat);
	vsprintf( wzLog ,wzMsgFormat,args);
	va_end(args);
	syslog(LOG_INFO,wzLog);

#endif

}

// -----------------------------------------------------------------------------
// Design Notes:  write log
//		
// Parameters:
//		wzMsgFormat: log message format;
// Returns:
//		void.
// -----------------------------------------------------------------------------
void CALog::LogBuffer(const char * name,const unsigned char* buffer, int length )
{
#ifdef WIN32
	// Maximal file size is 10M.
	// If it is exceed it, delete log file, and create an new empty file.
	DWORD dwFileSize = GetFileSize( m_hFile, NULL );
	if ( dwFileSize > 10 * 1024 * 1024 )
	{
		CloseHandle( m_hFile );
		DeleteFile( m_wzLogFilePath.c_str() );

		m_hFile = CreateFile( m_wzLogFilePath.c_str(),					// file to open
			GENERIC_READ | GENERIC_WRITE,				// open for writing
			FILE_SHARE_READ | FILE_SHARE_WRITE,		// share for reading and writing
			NULL,										// default security
			OPEN_ALWAYS,								// existing file only
			FILE_ATTRIBUTE_NORMAL,					// normal file
			NULL );									// no attr. template
	}

	CHAR szLog[MAX_LOG_LENGTH] = {0};
	int offset = 0;
	SYSTEMTIME st;

	GetLocalTime(&st);
	offset += sprintf( szLog, 
		"%4d-%d-%d %2d:%2d:%2d.%3d: %s,len:%2d,", 
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond,
		st.wMilliseconds,
		name,
		length);

	int i= 0;
	while (i < length)
	{
		offset += sprintf(szLog + offset,"%02x ",*(buffer+i));
		i++;
	}
	offset += sprintf(szLog + offset,"\r\n");
	
	DWORD dwNumberToWrite = offset;
	DWORD dwNumberWrite = 0;
	DWORD dwPos = SetFilePointer( m_hFile, 0, NULL, FILE_END );
	BOOL bRet = WriteFile( m_hFile, szLog, dwNumberToWrite, &dwNumberWrite, NULL );

#else
	char szLog[MAX_LOG_LENGTH] = {0};
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
	syslog(LOG_INFO,szLog);

#endif

}

