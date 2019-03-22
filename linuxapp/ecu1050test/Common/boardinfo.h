/*************************************************************************
	> File Name: boardinfo.h
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Mon 14 May 2018 01:51:01 PM CST
 ************************************************************************/
#ifndef BOARDINFO_H_
#define BOARDINFO_H_

#include <odm_dual_sim_board.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



int check_dual_sim_board(SIM_MODEM **sim_modem);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BOARDINFO_H_ */
