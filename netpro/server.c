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
	//noecho();
	printw("Listening...\n");
	refresh();

	int i32ConnectFD;
	struct sockaddr_storage ss;
	char path[50];
	char* rtcargv[16];
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
	getwd(path);
	printw("%s@",path);
	refresh();
	write(i32ConnectFD,path,strlen(path));

	while(1){
		int count = 0;
		char cmd[512];
		char res[10000];
		cmd[0] = '\0';
		res[0] = '\0';
		char ncmd[512];
		int rtcargc;
		int j;
		get_command_name(cmd, i32ConnectFD);
		//printw("%s", cmd);
		//refresh();
		clear();
		rtcargc=parse(cmd,rtcargv);
		if(strcmp(rtcargv[0],"exit")==0)
		{
			write(i32ConnectFD,"exit",4);
			endwin();
			return 1;
		}
		if(strcmp(rtcargv[0],"cd")!=0)
		{
			//doan nay la lua thay, he he
			strcpy(ncmd,rtcargv[0]);
			for(j=1;j<rtcargc;j++)
			{
				strcat(ncmd," ");
				strcat(ncmd,rtcargv[j]);
			}
			/*printw("%d\n",rtcargc);
			refresh();
			printw("%s\n",ncmd);
			refresh();*/
			get_command_result(res, ncmd);
			printw("%s", res);
			refresh();
			send_command_result(res, i32ConnectFD);
		}
		else
		{
			chdir(rtcargv[1]);
			getwd(path);
			strcat(path,"\1");
			write(i32ConnectFD,path,strlen(path));
			path[strlen(path)-1]='\0';
		}
		printw("\n%s@",path);
		refresh();
	}

	refresh();
	close(i32ConnectFD);
	endwin();
}

