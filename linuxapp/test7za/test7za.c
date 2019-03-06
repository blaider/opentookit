/*************************************************************************
	> File Name: test7za.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Thu 26 Oct 2017 09:17:38 AM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int skipline(FILE *f)
{
  int ch;
  do {
    ch = getc(f);
  } while ( ch != '\n' && ch != EOF );
  return 0;
}

int fix_zipfile_mode(char *path,char *filename)
{
	FILE * fd;
	char line[1024];
	char name[100];
	char fname[1024];

	if(access(filename,F_OK))
		return -1;

	sprintf(fname,"/media/net/7za l %s",filename);
	fd = popen(fname, "r");
	if(fd == NULL)
	{
		perror("df");
		return -1;
	}
	skipline(fd);
	skipline(fd);
	skipline(fd);
	skipline(fd);
	skipline(fd);
	skipline(fd);
	skipline(fd);
	skipline(fd);
	skipline(fd);
	skipline(fd);
	skipline(fd);
	skipline(fd);
	skipline(fd);
	skipline(fd);
	skipline(fd);
	while (fgets(line, sizeof(line), fd))
	{
		char name1[100];
		int count =	sscanf(line, "%*s %*s %*s %*s %*s %[^\n]\n", name);
		if(count > 0)
			printf("%s=%d=%s\n",__func__,count,name);
//		int len = strlen(line) - 1;
//		while(len)
//		{
//			if(line[len] == ' ')
//			{
//				strcpy(name,&line[len+1]);
//				break;
//			}else if(line[len] == '\r'||line[len] == '\n')
//			{
//				line[len]=0;
//			}
//
//			len--;
//		}
//		printf("%s\n",name);
	}
	fclose(fd);
	return 0;
}

int main(int argc,char *argv[])
{
	if(argc == 2)
	{
		printf("%s\n",argv[1]);
		fix_zipfile_mode(NULL,argv[1]);
	}
	return 0;
}
