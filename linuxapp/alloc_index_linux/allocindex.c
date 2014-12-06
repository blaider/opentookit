/*
 * =====================================================================================
 *
 *       Filename:  allocindex.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年12月06日 21时18分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BLOCK_SIZE 			16
#define BLOCK_NUMBER 	10
int main(int argc,char *argv[])
{
	int i;
	char *buf = (char *)malloc(BLOCK_SIZE*BLOCK_NUMBER);
	char *bufIndex[BLOCK_NUMBER];
	for(i=0;i<BLOCK_NUMBER;i++)
	{
		bufIndex[i] = buf + BLOCK_SIZE*i;
		printf("index:%d,%x\n",i,(unsigned int)bufIndex[i]);
	}
	return 0;
}
