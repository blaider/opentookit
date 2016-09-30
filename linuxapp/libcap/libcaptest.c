/*************************************************************************
	> File Name: libcaptest.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Tue 13 Sep 2016 01:30:23 PM CST
 ************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/capability.h>
#include <unistd.h>

static void print_usage(void)
{
	printf("usage: file1 file2");
}

int main(int argc,char *argv[])
{
	int i=0;
	if(argc < 2)
	{
		print_usage();
		return -1;
	}
	for( i=1;i<argc;i++ )
	{
		if(!access(argv[i],F_OK))
		{
			cap_t cap_d = cap_from_text( "cap_net_bind_service=+pe cap_sys_time=eip" );
			if (cap_d == NULL)
			{
				perror("fatal error");
			}
			int rc = cap_set_file( argv[i], cap_d );
			if(rc)
			{
				printf("set [%s] failed\n",argv[i]);
				perror("cap_set_file");
			}else{
				printf("set [%s] success\n",argv[i]);
			}
		}
		else{
			printf("File [%s] not exist\n",argv[i]);
		}
	}

	return 0;
}
