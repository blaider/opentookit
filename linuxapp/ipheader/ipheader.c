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
// File:   	 ipheader.c
// Author:  suchao.wang
// Created: Feb 26, 2016
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

int main()
{
	struct ipbp{
		uint8_t ihl:4,version:4;
	};

	char data = 0x45;
	struct ipbp *bp = (struct ipbp *)&data;
	printf("daa:%x,ihl:%x,version:%x\n",data,bp->ihl,bp->version);
}


