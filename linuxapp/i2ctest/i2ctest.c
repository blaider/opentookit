/*************************************************************************
	> File Name: i2ctest.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 04 Jul 2018 05:05:09 PM CST
 ************************************************************************/

#include<stdio.h>
#include <dlfcn.h>
#include "board_resource.h"

#define BOARDRESOURCE_FILE "libBoardResource.so"

int main(int argc,char *argv[])
{
	BRF_Board_Init Board_Init;
	BRF_Board_GetNodeID Board_GetNodeID;
	BRF_Board_DeInit Board_DeInit;

	void *dp;
	char *error;
	dp = dlopen( BOARDRESOURCE_FILE, RTLD_NOW );
	if ( dp == NULL )
	{
		return;
	}

	Board_Init = (BRF_Board_Init) dlsym( dp, "Board_Init" );
	error = dlerror();
	if ( error )
	{
		return;
	}

	Board_GetNodeID = (BRF_Board_GetNodeID) dlsym( dp, "Board_GetNodeID" );
	error = dlerror();
	if ( error )
	{
		return;
	}

	Board_DeInit = (BRF_Board_DeInit) dlsym( dp, "Board_DeInit" );
	error = dlerror();
	if ( error )
	{
		return;
	}

	printf("111\n");
	BR_HANDLE node_id_fd;
	int i = 0;
	unsigned int device_id = 0;
	BR_RESULT ret = Board_Init( &node_id_fd );
	while ( i < 5 && ret != BR_SUCCESS )
	{
		ret = Board_Init( &node_id_fd );
		sleep( 1 );
		i++;
	}
	if ( ret != BR_SUCCESS )
		return;
	printf("%d\n",node_id_fd);
	if ( node_id_fd > 0 )
	{
		ret = Board_GetNodeID( node_id_fd, &device_id );
		printf("broadcast nodeid:%d\n",device_id);
		if ( device_id == 0xffffffff )
		{
			printf("get device id error,%d\n",ret);
		}
	}
	printf("get end\n");
	Board_DeInit( node_id_fd );
	dlclose( dp );
	while(1)sleep(1);
}
