/*************************************************************************
 > File Name: getopt.c
 > Author: suchao.wang
 > Mail: suchao.wang@advantech.com.cn
 > Created Time: Wed 03 Dec 2014 07:42:29 PM CST
 ************************************************************************/

#include<stdio.h>

#include<unistd.h>

int main(int argc, char **argv)

{

	int ch;
	opterr = 0;

	while ((ch = getopt(argc, argv, "a:bcde")) != -1)
	{
		printf("optind:%d\n",optind);
		switch (ch)
		{

		case 'a':
			printf("option a:’%s’\n", optarg);
			break;

		case 'b':
			printf("option b :b\n");
			break;

		default:
			printf("other option :%c\n", ch);

		}

		printf("optopt +%c\n", optopt);

	}

	return 0;
}
