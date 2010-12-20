/*
 * client.c
 *
 *  Created on: Dec 3, 2010
 *  Author: PREDATOR TEAM
 */

#include "client_lib.h"
#include <ncurses.h>

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
	char line[512];
	char ch;
	char buffer[10000];
	int i = 0;
	printw("@ ");
	while (ch = getch()) {
		write(s, &ch, 1);
		if (ch == '\n') {
			buffer[0] = '\0';
			printw("\n");
			refresh();
			/*i = read(s, buffer, sizeof(buffer));
			if (buffer[0] != '\n') {
				buffer[i] = '\0';
				printw("%s\n", buffer);
			}*/
			while (read(s, &ch, 1) > 0){
				addch(ch);
			}
			refresh();
			printw("@ ");
		}
	}
	endwin();
}
