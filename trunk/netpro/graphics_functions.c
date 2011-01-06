/*
 * graphics_functions.c
 *
 *  Created on: Dec 20, 2010
 *      Author: abcd
 */

#include <ncurses.h>

//Copy from...
WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

void destroy_win(WINDOW *local_win)
{
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners
	 * and so an ugly remnant of window.
	 */
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	/* The parameters taken are
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window
	 * 3. rs: character to be used for the right side of the window
	 * 4. ts: character to be used for the top side of the window
	 * 5. bs: character to be used for the bottom side of the window
	 * 6. tl: character to be used for the top left corner of the window
	 * 7. tr: character to be used for the top right corner of the window
	 * 8. bl: character to be used for the bottom left corner of the window
	 * 9. br: character to be used for the bottom right corner of the window
	 */
	wrefresh(local_win);
	delwin(local_win);
}

void backspace(){
	int x,y;
	getyx(stdscr, y, x);
	if (x>0){
		move(y, x-1);
		addch(' ');
		move(y, x-1);
	}
}

void printInfo(){
	WINDOW *my_win;
	int startx, starty, width, height;
	int ch;

	//initscr();			/* Start curses mode 		*/
	//cbreak();			/* Line buffering disabled, Pass on
	 	 	 	 	 	// * everty thing to me 		*/
	//keypad(stdscr, TRUE);		/* I need that nifty F1 	*/

	height = 15;
	width = 30;
	//starty = (LINES - height) / 2;	/* Calculating for a center placement */
	//startx = (COLS - width) / 2;	/* of the window		*/
	starty = 0;
	startx = COLS - width - 5;
	//printw("Press F1 to exit");
	refresh();
	my_win = create_newwin(height, width, starty, startx);

}

/*int main(){
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
	int i;
	for (i=0; i<50; i++){
		printw("%d\n", i);
		refresh();
	}
	sleep(2);
	endwin();
}*/
