/*
 * graphics_functions.c
 *
 *  Created on: Dec 20, 2010
 *      Author: abcd
 */

#include <ncurses.h>

void backspace(){
	int x,y;
	getyx(stdscr, y, x);
	if (x>0){
		move(y, x-1);
		addch(' ');
		move(y, x-1);
	}
}

int main(){
	char ch;
	initscr();
	noecho();
	while ((ch = getch()) != '\n'){
		//printw("%d", ch);
		if (ch == 127){
			backspace();
			//backspace();
		}
		else addch(ch);
	}
	sleep(2);
	endwin();
}
