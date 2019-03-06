// AdamAIread.cpp : Defines the entry point for the application.
//
#include <stdio.h>
#include <stdlib.h>
#include "Log.h"

#include "inc/bdaqadamio.h"
#include "inc/compatibility.h"

static uint16 module_id = 0xff;
static char module_name[16];

int myprocHandle = 0;

static int OpenLib()
{
	myprocHandle = 0;

	//OpenDeviceLib
	if(ADAMDrvOpen(&myprocHandle) == Success)
		return Success;
	else
		return -99;
}

static void CloseLib()
{
	//Close device library
	if(myprocHandle!=0)
		ADAMDrvClose(&myprocHandle);
}

static int showModuleInfo()
{
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		if (SYS_GetModuleID(myprocHandle, i, &module_id) == Success)
			if (SYS_GetModuleName(myprocHandle, i, module_name) == Success)
				LOGEX("Slot: %2u ,Module ID: 0x%02x,Module Name: %s\n",i,module_id,module_name);
			}
	return 0;
}



//int aoaitest(uint16 ai,uint16 relay,uint16 ao);
int didotest();
int setdo();
int moduletest()
{
	int iRet;
	int errors = 0;

	iRet = OpenLib();
	if(iRet != Success)
	{
		printf("OpenLib(%d) failed.\n", iRet);
		return 0;
	}
	showModuleInfo();
//	if(aoaitest(0,1,2) != 0)
//	{
//		errors++;
//	}
	if(didotest() != 0)
	{
		errors++;
	}

	setdo();
	CloseLib();
	printf("/***        END         ***/ \n");
	return -errors;
}

