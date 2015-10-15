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
	for(int i=0;i<20;i++)
	{
		if(i == 2)
		{
			mgr->StartProgram();
			mgr->RenameFiles(listf);
		}
		printf("%d,%x\n",i,mgr->GetStatus().status);
		sleep(1);
	}
	return 0;
}
