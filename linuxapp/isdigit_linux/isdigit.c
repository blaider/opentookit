/*************************************************************************
 > File Name: isdigit.c
 > Author: suchao.wang
 > Mail: suchao.wang@advantech.com.cn
 > Created Time: Fri 05 Dec 2014 07:33:07 PM CST
 ************************************************************************/

#include<stdio.h>
#include<ctype.h>
/* 找出str字符串中为阿拉伯数字的字符*/
void parse(char c)
{
	if (isdigit(c))
		printf("%c is an digit character\n", c);
	if (isalnum(c))
		printf("%c is an isalnum\n", c);
	if (isalpha(c))
		printf("%c is an isalpha\n", c);
	if (iscntrl(c))
		printf("%c is an iscntrl\n", c);
	if (islower(c))
		printf("%c is an islower\n", c);
	if (isgraph(c))
		printf("%c is an isgraph\n", c);
	if (isprint(c))
		printf("%c is an isprint\n", c);
	if (ispunct(c))
		printf("%c is an ispunct\n", c);
	if (isspace(c))
		printf("%c is an isspace\n", c);
	if (isupper(c))
		printf("%c is an isupper\n", c);
	if (isxdigit(c))
		printf("%c is an isxdigit\n", c);
}

int main(int argc,char *argv[])
{
	char str[] = "123@#FDsP[e?";
	int i;
	for (i = 0; str[i] != 0; i++)
	{
		parse(str[i]);
	}
	return 0;
}
