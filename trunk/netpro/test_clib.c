/*
 * test_clib.c
 *
 *  Created on: Dec 7, 2010
 *      Author: PREDATOR TEAM
 */

#include "client_lib.h"
#include <ncurses.h>

int main(){
	int s = create_client_socket("12345", "localhost");

	if (s == -1){
		perror("*error connecting*");
		exit(1);
	}

	initscr();
	char line[512];
	char ch[1];
	char buffer[10000];
	int i = 0;
	printw("@ ");
	while (ch[0] = getch()){
		write(s, ch, 1);
		if(ch[0]=='\n')
		{
			buffer[0]='\0';
			printw("\n");
			refresh();
			i=read(s,buffer,sizeof(buffer));
			if(buffer[0]!='\n')
				{
					buffer[i]='\0';
					printw("%s\n",buffer);
				}
			refresh();
			printw("@ ");
		}
	}
	endwin();
}
