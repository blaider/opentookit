/*************************************************************************
	> File Name: ncursestest.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Tue 16 Dec 2014 03:48:08 PM CST
 ************************************************************************/

#include<stdio.h>
#include <unistd.h>
#include <ncurses.h>

int main(int argc,char *argv[])
{
	initscr();

	move(5,15);
	printw("%s","hello blaider");
	refresh();
	sleep(2);
	flash();
	sleep(2);

	endwin();
	return 0;
}
