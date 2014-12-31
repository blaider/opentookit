/*************************************************************************
	> File Name: ipmod.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Tue 16 Dec 2014 05:22:43 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>

#define PW_LEN 256
#define NAME_LEN 256

int main(int argc,char *argv[])
{
	char name[NAME_LEN];
	char password[PW_LEN];
	const char *real_password = "xyzzyx";
	int i = 0;

	initscr();
	move(5,10);
	printw("Please login:");

	move(7,10);
	printw("%s","User name:");
	getstr(name);

	move(8,10);
	printw("%s","Password: ");
	refresh();

	cbreak();
	noecho();
	memset(password,'\0',sizeof(password));
	while(i<PW_LEN)
	{
		password[i] = getch();
		if(password[i] == '\n') break;
		move(8,20 +i);
		addch('*');
		refresh();
		i++;

	}

	echo();
	nocbreak();

	move(11,10);
	if(strncmp(real_password,password,strlen(real_password)) == 0)
		printw("%s","Correct ");
	else printw("%s","Wrong ");
	printw("%s","password");
	refresh();
	sleep(2);

	endwin();
	exit(EXIT_SUCCESS);
	return 0;
}
