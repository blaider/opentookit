// #############################################################################
// *****************************************************************************
//                  Copyright (c) 2016, Advantech Automation Corp.
//      THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY
//               INFORMATION WHICH IS THE PROPERTY OF ADVANTECH AUTOMATION CORP.
//
//    ANY DISCLOSURE, USE, OR REPRODUCTION, WITHOUT WRITTEN AUTHORIZATION FROM
//               ADVANTECH AUTOMATION CORP., IS STRICTLY PROHIBITED.
// *****************************************************************************
// #############################################################################
//
// File:   	 netcard.c
// Author:  suchao.wang
// Created: Feb 17, 2016
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////



#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>

int main(int argc,char *argv)
{
	do
	{
		char data[1024];
		FILE *pnd;
		pnd = fopen("/proc/net/dev", "r");
		if (!pnd)
		{
			fprintf(stderr, "%s: /proc/net/dev: %s", "aa", strerror(errno));
			break;
		}
		//skip header
		int ch;
		do
		{
			ch = getc(pnd);
		} while (ch != '\n' && ch != EOF);
		do
		{
			ch = getc(pnd);
		} while (ch != '\n' && ch != EOF);

		char ifname[10];
		do
		{
			memset(ifname, 0, sizeof(ifname));
			int ret = fscanf(pnd, " %6[^:]:", ifname);

			if (ret <= 0)
				break;

//				  printf("ret = %d,find netcard :%s\n",ret,ifname);
			do
			{
				ch = getc(pnd);
			} while (ch != '\n' && ch != EOF);

			if (strcmp(ifname, "lo"))
				dataLength += pThis->GetCardInfo(ifname, data + dataLength);

			//		  fclose(pnd);
			//		  sleep(1);

		} while (1);
		fclose(pnd);

	} while (0);
}

