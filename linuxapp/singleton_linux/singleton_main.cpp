/*************************************************************************
	> File Name: time_main.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Fri 28 Nov 2014 03:57:02 PM CST
 ************************************************************************/

#include<stdio.h>
#include "DataProcessMgr.h"
#include "sharemem.h"


int main(int argc,char *argv[])
{
	DataProcessMgr *mgr = DataProcessMgr::GetInstance();
	list<string> listf;
	listf.push_back("abc");
	listf.push_back("cde");
	mgr->Initialize();
	PSHRINFO infos;
	ShareMemInit(&infos);
	infos->status.status = STAT_TIMEOUT;
	int index=0;
	while(1)
	{
		if(index == 1)
		{
			mgr->StopProgram();
			mgr->RenameFiles(listf);
		}
		if(index == 20)
			mgr->StartProgram();
		printf("index:%d,status:%d,index:%x,total:%d,time:%ld\n",index,mgr->GetStatus().status,mgr->GetStatus().index,mgr->GetStatus().max,mgr->GetStatus().modifytime);
		printf("index:%d,status:%d,index:%x,total:%d,time:%ld\n",index,infos->status.status,infos->status.index,infos->status.max,infos->status.modifytime);
		sleep(1);
		index++;
	}
	return 0;
}
