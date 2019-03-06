/*************************************************************************
	> File Name: dio.cpp
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 31 Jan 2018 02:08:02 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "Log.h"

#include "inc/bdaqadamio.h"
#include "inc/compatibility.h"

static int ADV_SUCCESS = 0;

extern int myprocHandle;


static char * range2char(uint8 i_byRange)
{
	char *str = NULL;
	switch(i_byRange)
	{
	case 0:
		str = (char *)"Bi_Direction";
		break;
	case 1:
		str = (char *)"Up_Down";
		break;
	case 2:
		str = (char *)"Frequency";
		break;

	default:
		str = (char *) "Unknown";
		break;
	}

	return str;
}

static int set_state(uint16 slot,int channel,bool bstate)
{
	int ret = 0;
	bool bVal;
	ret = CNT_SetState(myprocHandle,slot, channel,bstate);
	if(ret == 0)
	{
		printf("slot:%d,ch:%d CNT_SetState done.\n",slot, channel);
	}
	else
	{
		printf("slot:%d,ch:%d CNT_SetState failed.\n",slot, channel);
		return -1;
	}

	ret = CNT_GetState(myprocHandle, slot, channel, &bVal);
	if( ret == ADV_SUCCESS)
	{
		printf("slot:%d,ch:%d CNT_GetState[%d] done.\n",slot, channel,bVal);
	}else
	{
		printf("slot:%d,ch:%d CNT_GetState[%d] failed.\n",slot, channel,bVal);
		return -1;
	}
	return 0;
}

static int set_range(uint16 slot,int channel,uint8 i_byRange)
{
	int ret = 0;
	uint8 sVal;
	ret = CNT_SetRange(myprocHandle,slot, channel,i_byRange);
	if(ret == 0)
	{
		printf("slot:%d,ch:%d CNT_SetRange[%d]%s done.\n",slot, channel,i_byRange,range2char(i_byRange));
	}
	else
	{
		printf("slot:%d,ch:%d CNT_SetRange[%d]%s failed.\n",slot, channel,i_byRange,range2char(i_byRange));
		return -1;
	}

	ret = CNT_GetRange(myprocHandle, slot, channel, &sVal);
	if( ret == ADV_SUCCESS)
	{
		printf("slot:%d,ch:%d CNT_GetRange[%d]%s done.\n",slot, channel,sVal,range2char(sVal));
	}
	else
	{
		printf("slot:%d,ch:%d CNT_GetRange[%d]%s failed.\n",slot, channel,sVal,range2char(sVal));
		return -1;
	}
	return 0;
}

static void get_cnt_value(uint16 slot,int channel,uint32 *lVal)
{
	uint32 ret=0;
	ret = CNT_GetValue(myprocHandle, slot, channel, lVal);
	if( ret == 0)
	{
		printf("slot:%d,ch:%d CNT_GetValue[%d] done.\n",slot, channel,*lVal);
	}else
	{
		printf("slot:%d,ch:%d CNT_GetValue[%d] failed.\n",slot, channel,*lVal);
	}
}

static int do_set_value(uint16 slot,uint16 channel,bool benable)
{
	uint32 dVal = 0;
	bool bValue;
	//Set DO value
	if((DO_SetValue(myprocHandle, slot,channel, benable) == Success))
	{
		LOGEX("slot:%d,ch:%d DO_SetValue:[%d]0x%x done.\n",slot, channel,benable,benable);

		dVal = 0;
		//Check value
		if((DO_GetValues(myprocHandle, slot, &dVal)) == Success)
		{
			printf("slot:%d,ch:%d DO_GetValues:[%d]0x%x done.\n",slot, channel,dVal,dVal);
			bValue = ((dVal & (0x00000001<<channel)) > 0);
			if(bValue)
				printf("slot:%d,ch:%d enabled\n",slot, channel);
			else
				printf("slot:%d,ch:%d diabled\n",slot, channel);
		}
	}
	else
		printf("slot:%d,ch:%d DO_SetValue:[%d]0x%x failed.\n",slot, channel,dVal,dVal);
	return 0;

}

static int di_get_value(uint16 slot,uint16 channel,bool *benable)
{
	if((DI_GetValue(myprocHandle, slot,channel, benable) == Success))
	{
		printf("slot:%d,ch:%d DI_GetValue:[%d]0x%x done.\n",slot, channel,*benable,*benable);
	}
	else
		printf("slot:%d,ch:%d DI_GetValue:[%d]0x%x failed.\n",slot, channel,*benable,*benable);
	return 0;

}

#define SLOT_5080NEW 3
#define SLOT_5080OLD 4
#define SLOT_5081	5
#define SLOT_5052	6
#define SLOT_5056SO	7

static int check_device()
{
	uint16 module_id = 0xff;
	char module_name[16];
	//adam5017 check
	if (SYS_GetModuleID(myprocHandle, SLOT_5080NEW, &module_id) != Success)
		return -1;
	if (SYS_GetModuleName(myprocHandle, SLOT_5080NEW, module_name) != Success)
		return -1;

	if (strcmp(module_name, "ADAM5080") != 0)
		return -1;
/*
	//adam5069 check
	if (SYS_GetModuleID(myprocHandle, SLOT_5080OLD, &module_id) != Success)
		return -1;
	if (SYS_GetModuleName(myprocHandle, SLOT_5080OLD, module_name) != Success)
		return -1;

	if (strcmp(module_name, "ADAM5080") != 0)
		return -1;

	//adam5024 check
	if (SYS_GetModuleID(myprocHandle, SLOT_5081, &module_id) != Success)
		return -1;
	if (SYS_GetModuleName(myprocHandle, SLOT_5081, module_name) != Success)
		return -1;

	if (strcmp(module_name, "ADAM5080") != 0  && strcmp(module_name, "ADAM5081") != 0)
		return -1;
	//adam5024 check
	if (SYS_GetModuleID(myprocHandle, SLOT_5052, &module_id) != Success)
		return -1;
	if (SYS_GetModuleName(myprocHandle, SLOT_5052, module_name) != Success)
		return -1;

	if (strcmp(module_name, "ADAM5052") != 0)
		return -1;

	//adam5024 check
	if (SYS_GetModuleID(myprocHandle, SLOT_5056SO, &module_id) != Success)
		return -1;
	if (SYS_GetModuleName(myprocHandle, SLOT_5056SO, module_name) != Success)
		return -1;

	if (strcmp(module_name, "ADAM5056") != 0)
		return -1;
		*/
	return 0;
}

static int set_pulse(uint32 checkvalue)
{
	int errors=0;
	bool bval;
	uint32 lval = 0;
	do_set_value(SLOT_5056SO,1,true);
	usleep(100*1000);
	di_get_value(SLOT_5052,0,&bval);
	if(bval == true)
	{
		errors++;
		LOGEX("slot:%d,ch:%d DI_GetValue:[%d]0x%x ,expend=%d ,failed.\n",SLOT_5052, 0,bval,bval,false);
	}else{
		LOGEX("slot:%d,ch:%d DI_GetValue:[%d]0x%x ,expend=%d ,success.\n",SLOT_5052, 0,bval,bval,false);
	}


	do_set_value(SLOT_5056SO,1,false);
	usleep(100*1000);
	di_get_value(SLOT_5052,0,&bval);
	if(bval == false)
	{
		errors++;
		LOGEX("slot:%d,ch:%d DI_GetValue:[%d]0x%x ,expend=%d ,failed.\n",SLOT_5052, 0,bval,bval,true);
	}else{
		LOGEX("slot:%d,ch:%d DI_GetValue:[%d]0x%x ,expend=%d ,success.\n",SLOT_5052, 0,bval,bval,true);
	}
	get_cnt_value(SLOT_5080NEW,0,&lval);

	if(lval != checkvalue)
	{
		errors++;
		LOGEX("slot:%d,ch:%d DI_GetValue:[%d]0x%x ,expend=%d ,failed.\n",SLOT_5080NEW, 0,lval,lval,checkvalue);
	}else{
		LOGEX("slot:%d,ch:%d DI_GetValue:[%d]0x%x ,expend=%d ,done.\n",SLOT_5080NEW, 0,lval,lval,checkvalue);
	}

	get_cnt_value(SLOT_5080OLD,0,&lval);

	if(lval != checkvalue)
	{
		errors++;
		LOGEX("slot:%d,ch:%d DI_GetValue:[%d]0x%x ,expend=%d ,failed.\n",SLOT_5080OLD, 0,lval,lval,checkvalue);
	}else{
		LOGEX("slot:%d,ch:%d DI_GetValue:[%d]0x%x ,expend=%d ,done.\n",SLOT_5080OLD, 0,lval,lval,checkvalue);
	}
	get_cnt_value(SLOT_5081,0,&lval);
	if(lval != checkvalue)
	{
		errors++;
		LOGEX("slot:%d,ch:%d DI_GetValue:[%d]0x%x ,expend=%d ,failed.\n",SLOT_5081, 0,lval,lval,checkvalue);
	}else{
		LOGEX("slot:%d,ch:%d DI_GetValue:[%d]0x%x ,expend=%d ,done.\n",SLOT_5081, 0,lval,lval,checkvalue);
	}
	return -errors;
}


static int check_pulse(uint32 checkvalue)
{
	int errors=0;
	uint32 lval = 0;

	get_cnt_value(SLOT_5080NEW,0,&lval);

	if(lval == checkvalue)
	{
		errors++;
		LOGEX("slot:%d,ch:%d DI_GetValue:[%d]0x%x ,expend=%d ,failed.\n",SLOT_5080NEW, 0,lval,lval,checkvalue);
	}else{
		LOGEX("slot:%d,ch:%d DI_GetValue:[%d]0x%x ,expend=%d ,done.\n",SLOT_5080NEW, 0,lval,lval,checkvalue);
	}
	return -errors;
}


int didotest()
{
	int errosr = 0;
	int ret = 0;
	ret = check_device();
	if(ret)
		return ret;
	if(set_range(SLOT_5080NEW,0,ContrMod_UP_DOWN) != 0)
	{
		errosr++;
	}
//	if(set_range(SLOT_5080OLD,0,ContrMod_UP_DOWN) !=0)
//	{
//		errosr++;
//	}
//	if(set_range(SLOT_5081,0,ContrMod_UP_DOWN) != 0)
//	{
//		errosr++;
//	}
	if(set_state(SLOT_5080NEW,0,true) != 0)
	{
		errosr++;
	}
//	if(set_state(SLOT_5080OLD,0,true) != 0)
//	{
//		errosr++;
//	}
//	if(set_state(SLOT_5081,0,true) != 0)
//	{
//		errosr++;
//	}
//	set_state(SLOT_5081,0,true);
	sleep(1);
	if(check_pulse(100) != 0)
	{
		errosr++;
	}
	sleep(1);
	if (check_pulse(100) != 0)
	{
		errosr++;
	}
	sleep(1);
	if (check_pulse(100) != 0)
	{
		errosr++;
	}
	return -errosr;
}

int setdo()
{
	DO_SetValues(myprocHandle, SLOT_5056SO, 0xffff);
	DO_SetValues(myprocHandle, 1, 0xffff);
	return 0;
}

