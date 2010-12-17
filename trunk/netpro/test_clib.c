/*
 * test_clib.c
 *
 *  Created on: Dec 7, 2010
 *      Author: abcd
 */

#include "client_lib.h"
#include <ncurses.h>

int main(){
	int s = create_client_socket("12345", "localhost");

	if (s == -1){
		perror("*error connecting*");
		exit(1);
	}

/*	char buf[] = "qwerty1234";
	write(s, buf, sizeof(buf));
	read(s, buf, sizeof(buf));
	printf("receive from server: %s\n", buf);*/

	initscr();

	char ch[1];
	int i = 0;
	while ((ch[0] = getch()) != '\n'){
		write(s, ch, 1);
		/*sleep(1);
		printw("%c", ch);*/
	}
	write(s, ch, 1);

	endwin();
}
