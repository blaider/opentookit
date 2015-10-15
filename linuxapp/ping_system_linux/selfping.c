/********************************************************
 * PING
 * : GCC-4.2.4
 * YSQ-NJUST,yushengqiangyu@163.com
 *
 *******************************************************/

#include<stdio.h>
#include <errno.h>

int main()
{
	char cmd[] = "ping www.baidu.com -w 10 -c 1 -W 5";
	int i=0;
    char ch;
    for(i=0;i<10;i++)
    {
    	printf("%d:%s\n",i,cmd);
		int n = system(cmd);
		printf("result:%d\n",n);
		if(n)
			perror("system ping");
    	sleep(1);
    }
    return 0;
}

