/*
 * =====================================================================================
 *
 *       Filename:  hello.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年12月04日 22时16分03秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>

int main(int argc,char *argv[])
{
	int i;
	printf("Hello world!\n");
	for(i = 0;i<argc;i++)
	{
		printf("%s,",argv[i]);
	}
	printf("\n");
	return 0;
}
