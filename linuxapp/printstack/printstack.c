/*
 * =====================================================================================
 *
 *       Filename:  printstack.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年12月07日 20时42分20秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <elf.h>

int main(int argc, char*argv[])
{
	int *p = (int *)argv;
	int i;
	Elf32_auxv_t *aux;

	printf("argument count:%d\n",*(p-1));
	for(i = 0;i<*(p-1);++i)
	{
		printf("argument %d:%s\n",i,*(p+i));
	}
	p +=i;
	p++;
	printf("environment:\n");
	while(*p)
	{
		printf("%s\n",*p);
		p++;
	}
	p++;
	printf("auxiliary vecotrs:\n");
	aux = (Elf32_auxv_t *)p;
	while(aux->a_type!=AT_NULL)
	{
		printf("Type:%02d value:%x\n",aux->a_type,aux->a_un.a_val);
		aux++;
	}
	return 0;
}
