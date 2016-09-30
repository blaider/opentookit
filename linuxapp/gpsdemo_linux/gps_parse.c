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
// File:   	 gps_parse.c
// Author:  suchao.wang
// Created: Dec 11, 2015
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int parse_gprmc(const char *buf,int len)
{
	char * poss;
	char * pose;

	char buff[13][128];
	memset(buff,0,sizeof(buff));

	poss = (char *)buf;

	int i = 13;
	for(i=0;i<12;i++)
	{
		pose = strstr(poss+1,",");

		if(pose == NULL)
			return -1;
		if(pose - poss > 1)
			strncpy(buff[i],poss+1,pose-poss-1);

		poss = pose;
	}

	strcpy(buff[12],poss);

	for(i=0;i<13;i++)
			printf("%d-%s\n",i,buff[i]);

	return 0;

}


int main(int argc,char *argv[])
{
	int i = 0;
	char buff[]="$GPRMC,092427.604,V,4002.1531,N,11618.3097,E,0.000,0.00,280812,,E,N*08";
	char buff2[]="$GPRMC,,V,,,,,,,,,,N*53";
	char buf[13][128];
	memset(buf,0,sizeof(buf));
	sscanf(buff,"%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%s",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8],buf[9],buf[10],buf[11],buf[12]);

	for(i=0;i<13;i++)
		printf("%d-%s\n",i,buf[i]);

	memset(buf,0,sizeof(buf));
	sscanf(buff2,"%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%s",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8],buf[9],buf[10],buf[11],buf[12]);

	for(i=0;i<13;i++)
		printf("%d-%s\n",i,buf[i]);

	parse_gprmc(buff,strlen(buff));
	parse_gprmc(buff2,strlen(buff2));

	return 0;
}



