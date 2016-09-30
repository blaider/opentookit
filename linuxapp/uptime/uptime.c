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
// File:   	 uptime.c
// Author:  suchao.wang
// Created: May 17, 2016
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <time.h>



// 返回自系统开机以来的毫秒数（tick）
unsigned long GetTickCount()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    double time = ts.tv_sec+1.0*ts.tv_nsec/(1000*1000*1000);
    printf("%lf\n",time);
    return (ts.tv_sec);
}

int main()
{
	printf("%ld\n",GetTickCount());
	system("cat /proc/uptime");
}
