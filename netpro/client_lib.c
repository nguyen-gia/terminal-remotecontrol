/*
 * client_lib.c
 *
 *  Created on: Dec 3, 2010
 *  Author: PREDATOR TEAM
 */

#include "client_lib.h"
#include <ncurses.h>

int create_client_socket(char* server_port_number, char *server_ip_address){
	char *host, *port;
	struct addrinfo hints, *ai0, *ai;
	int s;
	int gai;

	host = server_ip_address;
	port = server_port_number;

	/* Resolve the addresses associated */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = 0;

	gai = getaddrinfo(host, port, &hints, &ai0);
	if (gai) {
		fprintf(stderr,
				"getaddrinfo(): %s port %s: %s\n",
				host, port, gai_strerror(gai));
		exit(1);
	}

	/* Loop for creating an active connection to the remote host */
	s = -1;
	for (ai = ai0; ai; ai = ai->ai_next) {
		s = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if (s == -1)
			continue;

		if (connect(s, ai->ai_addr, ai->ai_addrlen) == 0)
			break;

		close(s);
		s = -1;
	}

	/* release memory */
	freeaddrinfo(ai0);

	return s;
}
int run_client(int client_socket){
	int i;
	char check[15];
	i=read(client_socket,check,sizeof(check));
	check[i]='\0';
	printf("%s\n", check);
	/*if(strcmp(check,"controller")==0)
		run_ctrl_client(client_socket);
	else
		run_normal_client(client_socket);*/
}

int run_ctrl_client(int client_socket){
	initscr();
	noecho();
	char line[512];
	char ch;
	char buffer[10000];
	char path[50];
	int i = 0;
	i=read(client_socket,path,sizeof(path));
	path[i]='\0';
	printw("%s@ ",path);
	refresh();

	while (ch = getch()) {
		write(client_socket, &ch, 1);
		if (ch == 127){
			backspace();
		}
		else addch(ch);
		refresh();

		if (ch == '\n') {
			clear();
			buffer[0] = '\0';
			//printw("\n");
			refresh();
			i = read(client_socket, buffer, sizeof(buffer));

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
			printw("%s@ ",path);
			refresh();
		}
	}
	endwin();
}

int run_normal_client(int client_socket)
{
	initscr();
	noecho();
	char line[512];
	char ch;
	char buffer[10000];
	char path[50];

	int i = 0;
	i=read(client_socket,path,sizeof(path));
	path[i]='\0';
	printw("%s@ ",path);
	refresh();

	while (1) {
		read(client_socket, &ch, 1);
		if (ch == 127){
		backspace();
	}
	else addch(ch);
	refresh();

	if (ch == '\n') {
		clear();
		buffer[0] = '\0';
		//printw("\n");
		refresh();
		i = read(client_socket, buffer, sizeof(buffer));

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
			printw("%s@ ",path);
			refresh();
		}
	}
	endwin();

}
