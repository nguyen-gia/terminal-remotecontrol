/*
 * test_slib.c
 *
 *  Created on: Dec 7, 2010
 *      Author: PREDATOR TEAM
 */

#include "server_lib.h"
#include <ncurses.h>

int main(){
	/* main loop */
	int s = create_server_socket("12345");


	if (s < 0){
		printf("error while creating server socket\n");
		exit(0);
	}


	initscr();
	int i;
	int newfd;
	struct sockaddr_storage ss;
	socklen_t sslen;
	ssize_t cc;
	char hostbuf[NI_MAXHOST], buf[NI_MAXHOST + 2];
	int gni;
	char ch[1];
	FILE *fp;
	char command[512];
	int count=0;
	char line[512];
	char buffer[10000];
	printw("Listening...\n");
	refresh();

	//wait for client
	sslen = sizeof(ss);
	while(1){
		newfd = accept(s, (struct sockaddr *)&ss, &sslen);
		if (newfd == -1) {
			perror("accept");
			continue;
			}
		else
			break;
	}

	//print remote address
	gni = getnameinfo((struct sockaddr *)&ss, sslen,
			hostbuf, sizeof(hostbuf),
			NULL, 0,
			NI_NUMERICHOST);
	if (gni)
		strcpy(hostbuf, "???");	//XXX

	printw("%s@ ", hostbuf);
	refresh();

	for(;;){
		count = 0;
		command[0]='\0';
		buffer[0]='\0';

		//get command: char* get_command(newfd);
		while (1){
			read(newfd, ch, 1);
			printw("%s", ch);
			refresh();
			if (ch[0] == '\n') break;
			command[count++] = ch[0];
		}
		command[count] = '\0';

		// int get_command_result(char *cmd, char*res)
		if((fp=popen(command,"r"))==NULL)
		{
			printw("Error when execute this command\n");
			continue;
		}
		int k = 0;
		while(fgets(line,sizeof(line),fp)!=NULL)
		{
			if(k==0){
				strcpy(buffer,line);
				k++;
			}
			else {
				strcat(buffer,line);
			}
			refresh();
		}
		pclose(fp);

		printw("%s",buffer);
		refresh();

		// int send_result(char* res)
		if(buffer[0]!='\0')
			write(newfd,buffer,strlen(buffer));
		else
			{
			strcat(buffer,"\n");
			write(newfd,buffer,strlen(buffer));
			}

		printw("%s@ ", hostbuf);
		refresh();
	}

	refresh();
	close(newfd);
	endwin();
}
