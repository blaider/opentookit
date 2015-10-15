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
// File:   	 getspd.c
// Author:  suchao.wang
// Created: Jul 16, 2015
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <shadow.h>
/*
 * This program loops, reading a login name from standard
 * input and checking to see if it is a valid name. If it
 * is not valid, the entire contents of the name in the
 * password database are printed.
 */
int main(int argc, char** argv)
{
	struct spwd* sp;
	char buf[80];
	setpwent();
	while (gets(buf) != NULL)
	{
		if ((sp = getspnam(buf)) != (struct spwd *) 0)
		{
			printf("Valid login name is: %s\n", sp->sp_namp);
			printf("Valid login name is: %s\n", sp->sp_pwdp);
		}
		else
		{
			setspent();
			while ((sp = getspent()) != (struct spwd *) 0)
				printf("%s\n", sp->sp_namp);
		}
	}
	endspent();
	return ( EXIT_SUCCESS);
}

