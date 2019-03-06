#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <string.h>
#include "odm_dual_sim_board.h"

#define BOARD_FILE "/proc/board"

#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))

BOARD_DUAL_SIM boardops[] = BOARD_DUAL_SIM_INFO;

static SIM_MODEM  default_modem = {{3,15,1,0},{0,4,1,0}};

static int dual_sim_board(char * boardname,SIM_MODEM **sim_modem)
{
	int findboard = 0;
	size_t i;

//	printf("%s:board type:%x\n",__func__,type);
	for(i=0;i<ARRAYSIZE(boardops);i++)
	{
		printf("%s,%s\n",boardops[i].name,boardname);
		if(strcmp(boardops[i].name,boardname)==0)
		{
			findboard = 1;
			*sim_modem = &(boardops[i].modem);
			break;
		}
	}
	if(! findboard)
		*sim_modem = &default_modem;
	return findboard;
}

int check_dual_sim_board(SIM_MODEM **sim_modem)
{
	FILE *file;
	char board_name[32];
	size_t i=0;

	//only adam3600 old version have no this file
	if(access(BOARD_FILE,F_OK))
		return 0;

	file = fopen(BOARD_FILE,"r");
	if(file != NULL)
	{
		memset(board_name,0,sizeof(board_name));
		fgets(board_name,sizeof(board_name),file);
		fclose(file);
	}

	i = strlen(board_name);
	while(i)
	{
		if(board_name[i-1] == '\r' || board_name[i-1] == '\n')
			board_name[i-1] = '\0';
		i--;
	}

	if(dual_sim_board(board_name,sim_modem))
		return 1;

	return 0;
}



