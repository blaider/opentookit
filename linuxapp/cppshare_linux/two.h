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
// File:   	 two.h
// Author:  suchao.wang
// Created: Dec 23, 2015
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////


#ifndef TWO_H_
#define TWO_H_
#include<stdio.h>
#include<stdlib.h>
#include"base.h"
class two : public base
{
public:
	virtual int getType()   = 0;

public:
	int test();
};

#endif /* TWO_H_ */
