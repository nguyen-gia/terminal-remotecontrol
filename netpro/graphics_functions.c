/*
 * graphics_functions.c
 *
 *  Created on: Dec 20, 2010
 *      Author: abcd
 */

#include <ncurses.h>

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

//Typically in ncurses environment, backspace is also realized as normal key press
//We must rewrite a function that allow we turn back each time we typed wrong
void backspace(){
	int x,y;

	//get x,y coodinator on screen
	getyx(stdscr, y, x);

	//move back a character and put a white space on it
	if (x>0){
		move(y, x-1);
		addch(' ');
		move(y, x-1);
	}
}


//printInfo() : Print information about server and connected host
void printInfo(char *server_host, char* client_host[]){
	WINDOW *my_win;
	int startx, starty, width, height;

	height = 15;
	width = 30;
	starty = LINES-height;
	startx = COLS - width;

	my_win = create_newwin(height, width, starty, startx);
	mvwprintw(my_win, 1, 1, "Server: %s", server_host);
	wrefresh(my_win);
	int i, row=2;

	for(i=0;i<12;i++)
		if(client_host[i]!=NULL)
			{
				mvwprintw(my_win, row, 1 , "%s", client_host[i]);
				wrefresh(my_win);
				row++;
			}
}
