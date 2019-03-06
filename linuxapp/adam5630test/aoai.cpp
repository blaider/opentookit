// AdamAIread.cpp : Defines the entry point for the application.
//
#include <stdio.h>
#include <stdlib.h>
#include "Log.h"

#include "inc/bdaqadamio.h"
#include "inc/compatibility.h"

//static int ADV_SUCCESS = 0;

extern int myprocHandle;

static char * range2char(uint8 i_byRange)
{
	char *str = NULL;
	switch(i_byRange)
	{
	case V_Neg10To10:
		str = (char *)"V_Neg10To10";
		break;
	case V_Neg5To5:
		str = (char *)"V_Neg5To5";
		break;
	case V_Neg2pt5To2pt5:
		str = (char *)"V_Neg2pt5To2pt5";
		break;
	case V_Neg1To1:
		str = (char *)"V_Neg1To1";
		break;
	case mV_Neg500To500:
		str = (char *)"mV_Neg500To500";
		break;
	case mV_Neg150To150:
		str = (char *)"mV_Neg150To150";
		break;
	case mA_Neg20To20:
		str = (char *)"mA_Neg20To20";
		break;
	case mA_4To20:
		str = (char *)"mA_4To20";
		break;
	case V_0To2pt5:
		str = (char *)"V_0To2pt5";
		break;
	case V_0To5:
		str = (char *)"V_0To5";
		break;
	case V_0To10:
		str = (char *)"V_0To10";
		break;

	case V_0To1:
		str = (char *)"V_0To1";
		break;
	case V_0To15:
		str = (char *)"V_0To15";
		break;
	case V_Neg15To15:
		str = (char *)"V_Neg15To15";
		break;
	case mV_0To500:
		str = (char *)"mV_0To500";
		break;
	case mV_0To150:
		str = (char *)"mV_0To150";
		break;
	case mA_0To20:
		str = (char *)"mA_0To20";
		break;


    //5013

	case Pt385_Neg100To100:
		str = (char *)"Pt385_Neg100To100";
		break;

	case Pt385_0To100:
		str = (char *)"Pt385_0To100";
		break;

	case Pt385_0To200:
		str = (char *)"Pt385_0To200";
		break;

	case Pt385_0To600:
		str = (char *)"Pt385_0To600";
		break;

	case Pt392_Neg100To100:
		str = (char *)"Pt392_Neg100To100";
		break;

	case Pt392_0To100:
		str = (char *)"Pt392_0To100";
		break;


	case Pt392_0To200:
		str = (char *)"Pt392_0To200";
		break;

	case Pt392_0To600:
		str = (char *)"Pt392_0To600";
		break;

	case Ni518_Neg80To100:
		str = (char *)"Ni518_Neg80To100";
		break;

	case Ni518_0To100:
		str = (char *)"Ni518_0To100";
		break;
    //5018

	case mV_Neg15To15:
		str = (char *)"mV_Neg15To15";
		break;


	case mV_Neg50To50:
		str = (char *)"mV_Neg50To50";
		break;

	case mV_Neg100To100:
		str = (char *)"mV_Neg100To100";
		break;

	case Jtype_0To760C:
		str = (char *)"Jtype_0To760C";
		break;

	case Ktype_0To1370C:
		str = (char *)"Ktype_0To1370C";
		break;

	case Ttype_Neg100To400C:
		str = (char *)"Ttype_Neg100To400C";
		break;
	case Etype_0To1000C:
		str = (char *)"Etype_0To1000C";
		break;
	case Rtype_500To1750C:
		str = (char *)"Rtype_500To1750C";
		break;

	case Stype_500To1750C:
		str = (char *)"Stype_500To1750C";
		break;
	case Btype_500To1800C:
		str = (char *)"Btype_500To1800C";
		break;



	default:
		str = (char *) "Unknown";
		break;
	}

	return str;
}

static int check_device(uint16 ai,uint16 relay,uint16 ao)
{
	uint16 module_id = 0xff;
	char module_name[16];
	//adam5017 check
	if (SYS_GetModuleID(myprocHandle, ai, &module_id) != Success)
		return -1;
	if (SYS_GetModuleName(myprocHandle, ai, module_name) != Success)
		return -1;

	if (strcmp(module_name, "ADAM5017") != 0)
		return -1;

	//adam5069 check
	if (SYS_GetModuleID(myprocHandle, relay, &module_id) != Success)
		return -1;
	if (SYS_GetModuleName(myprocHandle, relay, module_name) != Success)
		return -1;

	if (strcmp(module_name, "ADAM5069") != 0)
		return -1;

	//adam5024 check
	if (SYS_GetModuleID(myprocHandle, ao, &module_id) != Success)
		return -1;
	if (SYS_GetModuleName(myprocHandle, ao, module_name) != Success)
		return -1;

	if (strcmp(module_name, "ADAM5024") != 0)
		return -1;
	return 0;
}

static int ai_setChEnable(uint16 slot,int channel)
{
	bool* bChEnabled = new bool[16];

	if(AI_GetChannelEnabled(myprocHandle, slot, bChEnabled) == Success)
	{

		bChEnabled[channel] = true;
		printf("slot:%d AI_GetChannelEnabled done.\n",slot);
		if(AI_SetChannelEnabled(myprocHandle, slot, bChEnabled) == Success)
		{
			printf("slot:%d,ch:%d AI_SetChannelEnabled done.\n",slot, channel);
		}
		else
		{
			printf("slot:%d,ch:%d AI_SetChannelEnabled failed.\n",slot, channel);
			return -1;
		}
	}else{
		printf("slot:%d AI_GetChannelEnabled failed.\n",slot);
		return -1;
	}
	return 0;
}

static int ai_getvalue(uint16 slot,int channel,double *fValue)
{
	uint32 ret=0;
	uint16 raw;
	ret = AI_GetValue(myprocHandle, slot,channel, &raw, fValue);

	if( ret == Success)
	{
		printf("slot:%d,ch:%d AI_GetValue:%lf[0x%X] done.\n",slot, channel, *fValue,raw);
	}
	else
	{
		printf("slot:%d,ch:%d AI_GetValue:%lf[0x%X] failed.\n",slot, channel, *fValue,raw);
	}
	return 0;
}

static int AoSetValueRange(uint16 slot,int channel,double fValue)
{
	if(AO_SetValue(myprocHandle, slot, channel, fValue) == Success)
	{
		printf("slot:%d,ch:%d AO_SetValue:[%lf] done.\n",slot, channel, fValue);
	}
	else
	{
		printf("slot:%d,ch:%d AO_SetValue:[%lf] failed.\n",slot, channel, fValue);
	}
	return 0;
}

static int ai_setting(uint16 slot, uint16 channel)
{
	ai_setChEnable(slot, channel);
	if (AI_SetInputRange(myprocHandle, slot, channel, (uint8) V_Neg10To10)
			== Success)
	{
		printf("slot:%d,ch:%d AI_SetInputRange:[%d]%s done.\n", slot, channel,
				V_Neg10To10, range2char(V_Neg10To10));
	}
	else
	{
		printf("slot:%d,ch:%d AI_SetInputRange:[%d]%s failed.\n", slot, channel,
				V_Neg10To10, range2char(V_Neg10To10));
		return -1;
	}
	return 0;
}

static int do_setting(uint16 slot,uint16 channel,bool benable)
{
	uint32 dVal = 0;
	bool bValue;
	//Set DO value
	if((DO_SetValue(myprocHandle, slot,channel, benable) == Success))
	{
		printf("slot:%d,ch:%d DO_SetValue:[%d]0x%x done.\n",slot, channel,benable,benable);

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

static int ao_setting(uint16 slot,uint16 channel)
{
	uint8 byRange = V_0To10;
	if(AO_SetOutputRange(myprocHandle, slot, channel, byRange) == Success)
	{
		printf("slot:%d,ch:%d AO_SetOutputRange:[%d]%s done.\n",slot, channel, V_Neg10To10,range2char(V_Neg10To10));
	}else
		printf("slot:%d,ch:%d AO_SetOutputRange:[%d]%s failed.\n",slot, channel, V_Neg10To10,range2char(V_Neg10To10));
	return 0;
}

#define AI_CHANNEL 6
#define RELAY_CHANNEL 6
#define AO_CHANNEL 2

int aoaitest(uint16 ai_slot,uint16 relay_slot,uint16 ao_slot)
{
	if(check_device(ai_slot,relay_slot,ao_slot))
		return -1;

	ai_setting(ai_slot,AI_CHANNEL);
	do_setting(relay_slot,RELAY_CHANNEL,true);
	ao_setting(ao_slot,AO_CHANNEL);

	double writed = 5.0;
	double readd = 0;
	AoSetValueRange(ao_slot,AO_CHANNEL,writed);
	sleep(1);
	ai_getvalue(ai_slot, AI_CHANNEL,&readd);
	double diff = writed-readd;
	if( diff > -0.5 && diff < 0.5)
	{
		LOGEX("AO %lf,AI %lf,diff %lf, test ok!\n",writed,readd,diff);
	}else{
		LOGEX("AO %lf,AI %lf,diff %lf, test failed!\n",writed,readd,diff);
	}


	do_setting(relay_slot,RELAY_CHANNEL,false);
	printf("wait 25 seconds\n");
	sleep(25);
	ai_getvalue(ai_slot, AI_CHANNEL,&readd);
	if( readd > -0.5 && readd < 0.5)
	{
		LOGEX("AO %lf,AI %lf, test ok!\n",writed,readd);
	}else{
		LOGEX("AO %lf,AI %lf, test failed!\n",writed,readd);
	}

	return 0;
}

