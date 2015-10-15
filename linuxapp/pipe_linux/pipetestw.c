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
// File:   	 pipetest.c
// Author:  suchao.wang
// Created: Mar 2, 2015
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>

void sig_pipe(int p)
{
	printf("pipe:%d",p);
}

typedef struct _command
{
	int cmd;
	int	paramter;
}CMD;

#define FIFOFILE "/tmp/wdtfifo0"
int main()
{
	signal(SIGPIPE,sig_pipe);
	int fd;
	char buffer[100];
	if (mkfifo(FIFOFILE, O_CREAT|O_EXCL) < 0 && errno != EEXIST)
	{
		fprintf(stderr, "Fail to mkfifo %s : %s.\n", FIFOFILE,
				strerror(errno));
//		exit(EXIT_FAILURE);
	}
	if ((fd = open(FIFOFILE, O_WRONLY )) < 0)
	{
		fprintf(stderr, "Fail to open %s : %s.\n", FIFOFILE,
				strerror(errno));
		exit(EXIT_FAILURE);
	}
	int j=0;
	CMD command;
	while(1)
	{
		memset(&command,0,sizeof(command));

		command.cmd=j++;
		command.paramter=j++;
		int n = write(fd,&command,sizeof(command));
		if(n<0)
			fprintf(stderr, "fd:%d,wirte %s : %s.\n",fd, FIFOFILE,
							strerror(errno));
		printf("write:%d,command:%d,%d\n",n,command.cmd,command.paramter);

		sleep(1);
	}
}
