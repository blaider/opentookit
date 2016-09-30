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
// File:   	 base.h
// Author:  suchao.wang
// Created: Dec 23, 2015
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////


#ifndef BASE_H_
#define BASE_H_
#include <stddef.h>

class base
{
public:
   virtual int getType()   = 0;
};


#endif /* BASE_H_ */
