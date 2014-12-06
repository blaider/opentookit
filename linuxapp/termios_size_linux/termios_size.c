/*************************************************************************
	> File Name: termios_size.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 03 Dec 2014 02:42:28 PM CST
 ************************************************************************/

#include<stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

int main( void )
{
    int width, height;
    struct winsize *ws;

    ws=(struct winsize*)malloc(sizeof(struct winsize));
    memset(ws,0x00,sizeof(struct winsize));
    ioctl(STDIN_FILENO,TIOCGWINSZ,ws);
    width=ws->ws_col;
    height=ws->ws_row;
    printf( "%d ", width );
    printf( "%d\n", height );

    return 0;
}
