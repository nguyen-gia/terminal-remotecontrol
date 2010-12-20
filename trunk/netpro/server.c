/*
 * server.c
 *
 *  Created on: Dec 3, 2010
 *  Author: PREDATOR TEAM
 */
#include "server_lib.h"
#include <ncurses.h>
#include <curses.h>

int main(int argc, char *argv[]){
	if(argc != 2){
		fprintf(stderr, "usage: %s [port-number]\n", argv[0]);
		exit(1);
	}

	int s = create_server_socket(argv[1]);
	if(s < 0){
		fprintf(stderr, "Error while creating socket\n");
		exit(2);
	}

	initscr();
	printw("Listening...\n");
	refresh();

	int i32ConnectFD;
	struct sockaddr_storage ss;
	socklen_t sslen = sizeof(struct sockaddr_storage);
	while(1){
		i32ConnectFD = accept(s, (struct sockaddr *)&ss, &sslen);
		if(i32ConnectFD < 0){
			perror("accept()");
			continue;
		}else
			break;
	}

	char hostbuf[NI_MAXHOST];
	int gni = getnameinfo((struct sockaddr *)&ss, sslen,hostbuf, sizeof(hostbuf),NULL, 0,NI_NUMERICHOST);
	printw("@");
	refresh();

	while(1){
		int count = 0;
		char cmd[512];
		char res[10000];
		cmd[0] = '\0';
		res[0] = '\0';

		get_command_name(cmd, i32ConnectFD);
		//printw("%s", cmd);
		//refresh();
		get_command_result(res, cmd);
		clear();
		printw("%s", res);
		refresh();

		send_command_result(res, i32ConnectFD);
		printw("\n@");
		refresh();
	}

	refresh();
	close(i32ConnectFD);
	endwin();
}

