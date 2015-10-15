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

#define FIFOFILE "fifos"
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
	if ((fd = open(FIFOFILE, O_RDONLY |O_NONBLOCK)) < 0)
	{
		fprintf(stderr, "Fail to open %s : %s.\n", FIFOFILE,
				strerror(errno));
//		exit(EXIT_FAILURE);
	}
	while(1)
	{
		memset(buffer,0,sizeof(buffer));

		int n = read(fd,buffer,sizeof(buffer));
		if(n > 0)
		{
			int i;
			printf("read:%d,",n);
			for(i=0;i<n;i++)
				printf("%02x ",buffer[i]);
			printf("\n");
		}
	}
}
