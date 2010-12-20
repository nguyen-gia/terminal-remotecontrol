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

/*	 process the arguments
	if (argc != 2 && argc != 3) {
		fprintf(stderr, "Usage: %s [host] portnum\n", argv[0]);
		exit(1);
	}*/

/*	if (argc == 3) {
		host = argv[1];
		port = argv[2];
	} else {
		host = NULL;	 Unspecified address
		port = argv[1];
	}*/
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

int get_command_name(char* cmd, int i32ConnectFD){
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
	//res = NULL;
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
  return rtcargc;
}
