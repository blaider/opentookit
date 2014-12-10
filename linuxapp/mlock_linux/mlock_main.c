/*************************************************************************
	> File Name: mlock_main.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 10 Dec 2014 11:22:34 AM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include<sys/mman.h>

const int alloc_size = 32 * 1024 * 1024;//分配32M内存
int main()
{
        char *memory = malloc(alloc_size);
        if(mlock(memory,alloc_size) == -1) {
                perror("mlock");
                return (-1);
        }
        size_t i;
        size_t page_size = getpagesize();
        printf("page_size:%d\n",page_size);
        for(i=0;i<alloc_size;i+=page_size) {
//                printf("i=%zd\n",i);
                memory[i] = 0;
        }

        if(munlock(memory,alloc_size) == -1) {
                perror("munlock");
                return (-1);
        }

        return 0;
}
