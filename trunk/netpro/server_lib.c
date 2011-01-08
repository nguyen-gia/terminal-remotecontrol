/*
 * server_lib.c
 *
 *  Created on: Dec 3, 2010
 *  Author: PREDATOR TEAM
 */

#include "server_lib.h"
#include <ncurses.h>

int create_server_socket(char *port_number){
	char *host, *port;
	struct addrinfo hints, *ai0, *ai;
	int gai;
	int s;
	int i;

	port = port_number;
	host = NULL;

	/* resolve the name */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	gai = getaddrinfo(host, port, &hints, &ai0);
	if (gai) {
		fprintf(stderr,
				"getaddrinfo(): %s port %s: %s\n",
				host != NULL ? host : "(null)",
				port,
				gai_strerror(gai));
		exit(1);
	}

	/* create a socket to accept connection from clients */
	s = -1;
	for (ai = ai0; ai; ai = ai->ai_next) {
		//int s;
		//printf("s = %d\n", s);

		/* create a socket */
		s = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if (s == -1)
			continue;

		/* bind the socket to the local port */
		if (bind(s, ai->ai_addr, ai->ai_addrlen) == -1) {
			close(s);
			s = -1;
			continue;
		}

		/* listen on the port */
		if (listen(s, 5) == -1) {
			close(s);
			s = -1;
			continue;
		}
		break;
	}

	/* free allocated memory */
	freeaddrinfo(ai0);

	return s;
}

int receive_command_name(char* cmd, int i32ConnectFD){
	char ch;
	int count = 0;
	cmd[0] = '\0';
	while(1){
		read(i32ConnectFD, &ch, 1);
		if (ch == 127){
			backspace();
			if (count > 0) count--;
		}
		else{
			printw("%c", ch);
			cmd[count++] = ch;
		}
		refresh();
		if(ch == '\n') break;
	}
	cmd[count] = '\0';

	return count;
}

int get_command_result(char *res, char *cmd){
	FILE *fp = NULL;
	if ((fp = popen(cmd, "r")) == NULL) {
		printw("Error when execute this command\n");
		refresh();
		return -1;
	}

	//printw("%s", cmd);
	//refresh();

	int k = 0;
	char line[512];

	while (fgets(line, sizeof(line), fp) != NULL) {
		if (k == 0) {
			sprintf(res, "%s", line);
			k++;
		} else {
			strcat(res,line);
		}
		refresh();
	}
	pclose(fp);

	return 0;
}

int send_command_result(char *res, int i32ConnectFD){
	if(res[0]!='\0'){
		write(i32ConnectFD, res, strlen(res));
		return 0;
	}else{
		strcat(res, "\n");
		write(i32ConnectFD, res, strlen(res));
		return -1;
	}
}

int parse(char *cmd, char *rtcargv[])
{
  int rtcargc;
  rtcargc = 0;
  while(*cmd&& *cmd<=' ')*cmd++='\0';
  while(*cmd)
    {
      rtcargv[rtcargc++] = cmd;
      while(*cmd && *cmd>' ') cmd++;
      while(*cmd && *cmd<=' ')
	*cmd++ = '\0';
    }
  rtcargv[rtcargc]= NULL;
  if(rtcargc==0) rtcargv[0]=" ";
  return rtcargc;
}

int acceptNewConnect(int serv_socket, char *hostbuf){
	int i32ConnectFD;
	struct sockaddr_storage ss;
	socklen_t sslen = sizeof(struct sockaddr_storage);
	while(1){
		i32ConnectFD = accept(serv_socket, (struct sockaddr *)&ss, &sslen);
		if(i32ConnectFD < 0){
			perror("accept()");
			continue;
		}else
			break;
	}

	int gni = getnameinfo((struct sockaddr *)&ss, sslen,hostbuf, sizeof(hostbuf),NULL, 0,NI_NUMERICHOST);

	return i32ConnectFD;
}

/*
 * This function use select() to handle multi-client connection
 * Input: server socket descriptor
 * Return value:
 */
int run_server(int serv_socket){
	initscr();
	//noecho();
	printw("Listening...\n");
	refresh();

	//accept new connect and get the client host
	char hostbuf[NI_MAXHOST];
	int i32ConnectFD = acceptNewConnect(serv_socket, hostbuf);

	char path[50];

	// get the current path
	getcwd(path, sizeof(path));
	refresh();
	write(i32ConnectFD, path, strlen(path));

	// get localhost name
	char server_host[256];
	gethostname(server_host, sizeof(server_host));

	// (graphics_function.c) print information about current connection state
	printInfo(server_host);

	while(1){
		printw("%s@ ",path);	// print the current path before each command is typed
		refresh();

		char cmd[512];
		char res[10000];
		cmd[0] = '\0';
		res[0] = '\0';
		char ncmd[512];
		char* rtcargv[16];
		int rtcargc;
		int j;

		// receive and display each character from client and combine into one command
		receive_command_name(cmd, i32ConnectFD);
		clear();

		// split cmd into array of command name and arguments
		rtcargc=parse(cmd,rtcargv);

		// in case of exit
		if(strcmp(rtcargv[0],"exit")==0)
		{
			write(i32ConnectFD,"exit",4);
			close(i32ConnectFD);
			endwin();
			return 1;
		}

		if(strcmp(rtcargv[0],"cd")!=0)
		{	// execute normal command
			//doan nay la lua thay, he he
			strcpy(ncmd,rtcargv[0]);
			for(j=1;j<rtcargc;j++)
			{
				strcat(ncmd," ");
				strcat(ncmd,rtcargv[j]);
			}

			// execute and store the result as a string 'res'
			get_command_result(res, ncmd);

			// print the result if exist
			if(res[0]!='\n') printw("%s\n", res);
			refresh();

			// then send the result to client via socket in order to display on client side
			send_command_result(res, i32ConnectFD);
		}
		else
		{	//in case of 'cd' command
			chdir(rtcargv[1]);			// execute the change dir
			getcwd(path, sizeof(path)); // get current path
			strcat(path,"\1");			// mark the string 'path' before send to client
			write(i32ConnectFD,path,strlen(path));	// send to client to change current path on client side too
			path[strlen(path)-1]='\0';	// remove the mark
		}
		printInfo(server_host);
		refresh();
	}

	//not reach
	refresh();
	close(i32ConnectFD);
	endwin();
}
