/*
 * client.c
 *
 *  Created on: Dec 3, 2010
 *  Author: PREDATOR TEAM
 */

#include "client_lib.h"
#include <ncurses.h>
#include <curses.h>

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "usage: %s [ip-address] [port-number]\n", argv[0]);
		exit(0);
	}

	int s = create_client_socket(argv[2], argv[1]);
	if (s < 0) {
		perror("socket()");
		exit(1);
	}

	initscr();
	noecho();
	char line[512];
	char ch;
	char buffer[10000];
	char path[50];
	int i = 0;
	i=read(s,path,sizeof(path));
	path[i]='\0';
	printw("%s @",path);
	while (ch = getch()) {
		write(s, &ch, 1);
		if (ch == 127){
			backspace();
		}
		else addch(ch);
		if (ch == '\n') {
			clear();
			buffer[0] = '\0';
			//printw("\n");
			refresh();
			i = read(s, buffer, sizeof(buffer));

			if (buffer[0] != '\n') {
				buffer[i] = '\0';
				if(strcmp(buffer,"exit")==0)
					{
					endwin();
					return 1;
					}
				if(buffer[strlen(buffer)-1]=='\1')
							{
								buffer[strlen(buffer)-1]='\0';
								strcpy(path,buffer);
							}
				else
				{
					printw("%s\n", buffer);
					refresh();
				}
			}

			//while (read(s, &ch, 1) > 0){
				//addch(ch);
			//}
			printw("%s @",path);
		}
	}
	endwin();
}
