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
// File:   	 awctest.c
// Author:  suchao.wang
// Created: Jun 25, 2015
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#include <AwcModule.h>

#define LOGEX printf

int main()
{
	char szPath[PATH_MAX];
	char filePath[PATH_MAX];
	int i;
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	void *handle;
	size_t rslt = readlink("/proc/self/exe", szPath, sizeof(szPath) - 1);
	if (rslt < 0 || (rslt >= sizeof(szPath) - 1)) {
		return 0;
	}
	szPath[rslt] = '\0';
	for (i = rslt; i >= 0; i--) {
		if (szPath[i] == '/') {
			szPath[i + 1] = '\0';
			break;
		}
	}
	if ((dp = opendir(szPath)) == NULL) {
		fprintf(stderr, "cannot open directory: %s\n", szPath);
		return 0;
	}
	chdir(szPath);
	while ((entry = readdir(dp)) != NULL) {
		lstat(entry->d_name, &statbuf);
		if (S_ISREG(statbuf.st_mode)) {
			if ((strstr(entry->d_name, ".so") != NULL) && (strstr(entry->d_name, "awc_") != NULL))
			{
				/*
				 打开某个动态库，注意检查打开是否成功
				 */
				strcpy(filePath,szPath);
				strcat(filePath,entry->d_name);
				handle = dlopen(filePath, RTLD_LAZY);
				if (!handle) {
					LOGEX("open %s failed\nERROR:%s", filePath , dlerror());
					continue;
				}

				PAWC_CONNECTION wlan_conn;
				PAWC_CONNECTION (*GetConnection)();
				GetConnection = dlsym(handle, "GetConnection");
				if (GetConnection != 0) {
					wlan_conn = GetConnection();
					LOGEX( "Load module %s, netcard %s, %s\n", entry->d_name, wlan_conn->netCardName,wlan_conn->processName);
				}else
				{
					LOGEX( "Get Module %s failed\n" , entry->d_name);
				}

			}
		}
	}
	closedir(dp);
}
