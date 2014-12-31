/*************************************************************************
	> File Name: ncursestest.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Tue 16 Dec 2014 03:48:08 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>

int main(int argc,char *argv[])
{
	const char witch_one[] = " First Witch   ";
	const char witch_two[] = " Second Witch  ";
	const char *scan_ptr;
	initscr();

	move(5,15);
	attron(A_BOLD);
	printw("%s","hello blaider");
	attroff(A_BOLD);
	refresh();
	sleep(1);

	move(8,15);
	attron(A_STANDOUT);
	printw("%s", "Thunder and Lightning");
	attroff(A_STANDOUT);
	refresh();
	sleep(1);

	move(10,10);
	printw("%s","when shall we three meet again");
	move(11,23);
	printw("%s","in thunder,lightning,or in rain?");
	move(13,10);
	printw("%s","when the hurlyburly's done,");
	move(14,23);
	printw("%s","when the battle's lost and won.");
	refresh();
	sleep(1);

	attron(A_DIM);
	scan_ptr = witch_one + strlen(witch_one) - 1;
	while(scan_ptr != witch_one)
	{
		move(10,10);
		insch(*scan_ptr--);
	}

	scan_ptr = witch_two + strlen(witch_two) - 1;
	while(scan_ptr != witch_two)
	{
		move(13,10);
		insch(*scan_ptr--);
	}

	attroff(A_DIM);
	refresh();
	sleep(1);

	move(LINES -1,COLS -1);

	refresh();
	sleep(10);


	endwin();
	exit(EXIT_SUCCESS);
	return 0;
}
