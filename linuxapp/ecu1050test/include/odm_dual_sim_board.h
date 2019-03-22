/*************************************************************************
	> File Name: odm_dual_sim_board.h
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Mon 14 May 2018 01:59:53 PM CST
 ************************************************************************/
#ifndef ODM_DUAL_SIM_BOARD_INFO_H
#define ODM_DUAL_SIM_BOARD_INFO_H


typedef struct pin_info
{
	int gpio_port;
	int gpio_index;
	int enable;
	int disable;

}PIN_INFO;

typedef struct board_sim_modem
{
	PIN_INFO power_pin;
	PIN_INFO sim_select;

}SIM_MODEM;

typedef struct board_dual_sim
{
	char *name;
	SIM_MODEM modem;

}BOARD_DUAL_SIM;

#define BOARD_DUAL_SIM_INFO \
{\
		{"ADAM3600DS",{{3,15,1,0},{0,4,1,0}}},\
		{"ECU1051",{{3,15,1,0},{2,25,0,1}}},\
}

#endif
