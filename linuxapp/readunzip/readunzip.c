/*************************************************************************
	> File Name: readunzip.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 13 Apr 2016 01:46:09 PM CST
 ************************************************************************/

#include<stdio.h>
#include <unistd.h>

int skipline(FILE *f)
{
  int ch;
  do {
    ch = getc(f);
  } while ( ch != '\n' && ch != EOF );
  return 0;
}

int checkAppOnly(char *filename)
{
	int find = 0;
	int findOther = 0;
	FILE * fd;
	char line[1024];
	char name[8][100];
	char fname[1024];

	if(access(filename,F_OK))
		return -1;

	sprintf(fname,"unzip -v  %s",filename);
	fd = popen(fname, "r");
	if(fd == NULL)
	{
		perror("df");
		return -1;
	}
	skipline(fd);
	skipline(fd);
	skipline(fd);
	while (fgets(line, sizeof(line), fd))
	{
		int count =	sscanf(line, "%s %s %s %s %s %s %s %s", name[0], name[1], name[2], name[3], name[4], name[5], name[6], name[7]);
//		printf("%s==%s\n",name[0],name[5]);
		if(count == 8)
		{
			if(!strcmp(name[7],"apps.tar.gz")||!strcmp(name[7],"manifest.xml"))
			{
				find++;
			}else
			{
				find = 0;
				break;
			}
		}else
			break;
//		printf("%d,%s \n",count,name[7]);
	}
	fclose(fd);
	printf("%d,%d \n",find,findOther);
	return !(find==2);
}

int main(int argc,char *argv[])
{
	int ret = checkUDisk(argv[1]);
	printf("ret = %d\n",ret);

	return 0;
}
