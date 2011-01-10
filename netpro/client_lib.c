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

int receive_connection_info(int client_socket, char *path, char* server_host, char *client_hosts[]){
	int i;
	for (i=0; i<12; i++) client_hosts[i] = NULL;

	//char *buf;
	//buf = (char*)malloc(1000);
	char bufs[1000];

	i = read(client_socket, bufs, sizeof(bufs));
	bufs[i] = '\0';

	char *buf = bufs;

	//
	printw("%s\n", buf);
	refresh();

	i = 0;
	while (buf[i] != '|' && i<strlen(buf)) i++;
	buf[i] = '\0';
	strcpy(path, buf);

	buf = &(buf[i+1]);
	i = 0;
	while (buf[i] != '|' && i<strlen(buf)) i++;
	buf[i] = '\0';
	strcpy(server_host, buf);

	//
	printw("%s\n", server_host);
	refresh();

	while(1){
		buf = &(buf[i+1]);
		if (strlen(buf) == 0) break;
		i = 0;
		while (buf[i] != '|' && i<strlen(buf)) i++;
		buf[i] = '\0';
		int index = atoi(buf);

		client_hosts[index] = (char*)malloc(strlen(buf) + 1);

		buf = &(buf[i+1]);
		i = 0;
		while (buf[i] != '|' && i<strlen(buf)) i++;
		buf[i] = '\0';
		strcpy(client_hosts[index], buf);

		printw("%s\n", client_hosts[index]);
		refresh();
	}

}

int run_client(int client_socket){
	int i,j,count,k;
	char check[15];
	char buf[3];
	initscr();
	noecho();
	i=read(client_socket,check,sizeof(check));
	check[i]='\0';
	printw("Read check\n");
	//printf("%s\n", check);
	if(strcmp(check,"controller")==0)
		j=2;
	else
		j=3;

	char firstpath[50];
	char server_host[50];
	char *client_hosts[12];

	receive_connection_info(client_socket, firstpath, server_host, client_hosts);


	/*i=read(client_socket,firstpath,sizeof(firstpath));
	firstpath[i]='\0';
	printw("Read path\n");

	i=read(client_socket,server_host,sizeof(server_host));
	server_host[i]='\0';
	printw("read server host\n");*/

	/*i=read(client_socket,buf,sizeof(buf));
	buf[i]='\0';
	count=atoi(buf);
	printw("read count = %d\n",count);
	for(k=0;k<count;k++)
	{
		i=read(client_socket,buf,sizeof(buf));
		buf[i]='\0';
		i=read(client_socket,hostbuf,sizeof(hostbuf));
		hostbuf[i]='\0';
		client_hosts[atoi(buf)]=(char*)malloc(strlen(hostbuf)+1);
		strcpy(client_hosts[atoi(buf)],hostbuf);
	}*/
	while(1)
	{
		if(j==1) return 1;
		if(j==2)
			j=run_ctrl_client(client_socket, firstpath, client_hosts, server_host);
		else
			j=run_normal_client(client_socket, firstpath, client_hosts, server_host);
	}
}
int run_ctrl_client(int client_socket, char* firstpath,char* clients_hosts[],char * server_host){
	char line[512];
	char ch;
	char buffer[10000];
	char path[50];
	int i;

	printw("You are controller\n");
	strcpy(path,firstpath);
	printw("%s@ ",path);
	printInfo(server_host,clients_hosts);
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
				else
				{
					if(strcmp(buffer,"changed")==0)
						return 3;
					else
					{
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
				}
			}
			printw("%s@ ",path);
			printInfo(server_host,clients_hosts);
			refresh();
		}
	}
	endwin();
}

int run_normal_client(int client_socket,char* firstpath, char* clients_hosts[], char* server_host)
{
	char line[512];
	char ch;
	char buffer[10000];
	char path[50];
	int i;

	printw("You are normal client\n");
	strcpy(path,firstpath);
	printw("%s@ ",path);
	printInfo(server_host,clients_hosts);
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
		refresh();
		i = read(client_socket, buffer, sizeof(buffer));

			if (buffer[0] != '\n') {
				buffer[i] = '\0';
				if(strcmp(buffer,"exit")==0)
				{
					endwin();
					return 1;
				}
				else
				{
					if(strcmp(buffer,"changed")==0)
						printw("Control permision has changed\n");
					else
						{
						if(strcmp(buffer,"controller")==0)
							return 2;
						else
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
					}
			}
			printw("%s@ ",path);
			printInfo(server_host,clients_hosts);
			refresh();
		}
	}
	endwin();

}
