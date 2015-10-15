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
// File:   	 systeminfo.cpp
// Author:  suchao.wang
// Created: Oct 27, 2014
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <errno.h>
#include <sys/mount.h>
#include <dlfcn.h>
#include <pthread.h>
#include <semaphore.h>

//nodeid
#define DIRS "/dev/serial/by-id/"
#define LABLE "usb-u-blox_u-blox_Wireless_Module"
#define PRE_LINK "../../"

int get_gprs_config_node_name(const char *lable,int port,char *result,int resultLen)
{
	int ret = 0;
	memset(result,0,resultLen);
	FILE * fd;
	char line[4096];
	char name[4096],linkName[4096];
	sprintf(name,"ls %susb-%s*",DIRS,lable);
	fd = popen(name, "r");
	if(fd == NULL)
	{
		perror(DIRS);
		return -1;
	}

	int inx = 0;

	while (fgets(line, sizeof(line), fd))
	{
//		printf("read:%s",line);
		sprintf(name,"%s",line);
		if(name[strlen(name)-1] == 0xa || name[strlen(name)-1] == 0xd)
			name[strlen(name)-1] = 0x0;
		memset(linkName,0,sizeof(linkName));
		int linkSize = readlink(name,linkName,sizeof(linkName));
		if(-1 == linkSize)
		{
//			perror("readlink");
			ret = -1;
			continue;
		}
//		printf("%s %s %s\n",name,(linkSize > 0)?" ->":"",linkName);
		if(port == inx )
		{

			if (strncmp(linkName, PRE_LINK, strlen(PRE_LINK)) == 0)
			{
				sprintf(name, "/dev/%s", linkName + strlen(PRE_LINK));
				if (resultLen > strlen(name) + 1)
				{
					memcpy(result, name, strlen(name));
					ret = strlen(name);
				}
			}
			break;

		}
		inx++;
	}
	pclose(fd);
	return ret;
}

int main(int argc,char *argv[])
{
	char buf[1024];
	if(get_gprs_config_node_name("u-blox_u-blox_Wireless_Module",3,buf,sizeof(buf)) > 0)
		printf("%s\n",buf);
}



