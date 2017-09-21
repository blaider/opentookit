/*************************************************************************
	> File Name: dir_sort.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Sat 19 Nov 2016 03:30:17 AM CST
 ************************************************************************/

#include <sys/types.h>
#include <dirent.h>

#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

//扫描所有的lib打头的文件

 int filter_fn(const struct dirent * ent)
 {
   if(ent->d_type != DT_REG)
     return 0;

   return (strncmp(ent->d_name,"lib",3) == 0);
 }


void scan_lib(char * dir_name)
{
  int n;
   struct dirent **namelist; // struct dirent * namelist[];

   n = scandir(dir_name, &namelist, NULL, alphasort);
   if (n < 0)
        perror("scandir");
   else {
	   while(n--) {
		   printf("%s\n", namelist[n]->d_name);
		   free(namelist[n]);
	   }
	   free(namelist);
   }
}

int main(int argc ,char * argv[])
{
   scan_lib("/home/sysuser/lib");
}

