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

int receive_command_name(char* cmd, int i32ConnectFD, fd_set*init, int maxfd, int serv_socket){
	char ch;
	int count = 0;
	cmd[0] = '\0';
	//Read sequently characters of command and send to normal client
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
		//Send to normal client
		int i;
		for (i=0; i<=maxfd; i++) if (i!=serv_socket && i!=i32ConnectFD && FD_ISSET(i, init)){
			write(i, &ch, 1);
		}
		refresh();
		if(ch == '\n') break;
	}
	cmd[count] = '\0';

	return count;
}

int get_command_result(char *res, char *cmd){
	FILE *fp = NULL;
	//Execute command and get result in file fp
	if ((fp = popen(cmd, "r")) == NULL) {
		printw("Error when execute this command\n");
		refresh();
		return -1;
	}

	int k = 0;
	char line[512];

	//Add content of result file to one buffer
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
	//Send command result to client ( both of controller and normal)
	if(res[0]!='\0'){
		write(i32ConnectFD, res, strlen(res));
		return 0;
	}else{
		strcat(res, "\n");
		write(i32ConnectFD, res, strlen(res));
		return -1;
	}
}

//parse command to array of parameter
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
	//This function is used to accept connect request from client and get client hostname
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

	/*int gni = */
	getnameinfo((struct sockaddr *)&ss, sslen, hostbuf, 256,NULL, 0,NI_NUMERICHOST);
	return i32ConnectFD;
}

void add_client_host(char *client_hosts[], int newfd, char* hostbuf){
	//This function is used to add client hostname to an array to print out information about connection
	client_hosts[newfd] = (char*)malloc(strlen(hostbuf) + 1);
	strcpy(client_hosts[newfd], hostbuf);
}

int receive_and_run(int* ctrl_sock_fd, int serv_socket, int max_sock_fd, fd_set* fds_init_addr, char*path, char* client_hosts[], int* mark)
{
	//Receive command and execute, send to client
	char cmd[512];
	char res[10000];
	cmd[0] = '\0';
	res[0] = '\0';
	char ncmd[512];
	char* rtcargv[16];
	int rtcargc;
	int j;

	// receive and display each character from client and combine into one command
	receive_command_name(cmd, *ctrl_sock_fd, fds_init_addr, max_sock_fd, serv_socket);
	clear();

	// split cmd into array of command name and arguments
	rtcargc=parse(cmd,rtcargv);

	//In case of there is an update of connection
	if(*mark>0){
			char info[60];
			if(client_hosts[*mark]!=NULL)
				sprintf(info,"%c%d|%s",'\2',*mark,client_hosts[*mark]);
			else
				sprintf(info,"%c%d|",'\2',*mark);
			for (j=0; j<=max_sock_fd; j++)
				if (j!=serv_socket && FD_ISSET(j, fds_init_addr)&& j!=*mark){
					write(j,info,strlen(info));
					}
			*mark=0;
		}
	// in case of exit
	if(strcmp(rtcargv[0],"exit")==0)
	{
		for (j=0; j<=max_sock_fd; j++)if (j!=serv_socket && FD_ISSET(j, fds_init_addr)){
			write(j,"exit",4);
		}
		close(*ctrl_sock_fd);
		endwin();
		return 1;
	}
	//in case of change controller permision
	if(strcmp(rtcargv[0],"change")==0)
	{
		int i,count=0;
		int old;
		int order;
		order=atoi(rtcargv[1]);
		for(i=0;i<=12;i++)
			{
			if(client_hosts[i]!=NULL)
				count++;
			if(count==order) break;
			}
			old = *ctrl_sock_fd;
			*ctrl_sock_fd = i;
			for (j=0; j<=max_sock_fd; j++)if (j!=serv_socket && FD_ISSET(j, fds_init_addr)){
				if(j!=*ctrl_sock_fd) write(j,"changed",strlen("changed")+1);
				else  write(j,"controller",strlen("controller")+1);
			}
	}
	else
	{
		if(strcmp(rtcargv[0],"cd")!=0)
		{
			// execute normal command
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
			for (j=0; j<=max_sock_fd; j++)if (j!=serv_socket && FD_ISSET(j, fds_init_addr)){
				send_command_result(res, j);
			}
		}
		else
		{	//in case of 'cd' command
			chdir(rtcargv[1]);			// execute the change dir
			getcwd(path, 50); // get current path
			strcat(path,"\1");			// mark the string 'path' before send to client

			for (j=0; j<=max_sock_fd; j++)if (j!=serv_socket && FD_ISSET(j, fds_init_addr)){
				write(j,path,strlen(path));	// send to client to change current path on client side too
			}
			path[strlen(path)-1]='\0';	// remove the mark
		}

		return 0;
	}
}

int send_client_hosts(char* client_hosts[], int newfd){
	//This function is used to send connection information to new connection
	int i,num = 0;
	for (i=0; i<12; i++) if (client_hosts[i] != NULL){
		num++;
	}
	char index[3];
	sprintf(index, "%d", num);
	write(newfd, index, sizeof(index));

	for (i=0; i<12; i++) if (client_hosts[i] != NULL){
		sprintf(index, "%d", i);
		write(newfd, index, sizeof(index));
		write(newfd, client_hosts[i], sizeof(client_hosts[i]));
	}

	return 0;
}
int send_connection_info(int newfd, char *path, char *server_host, char *client_hosts[]){
	//This function is used to send connection information to old connection
	char info[1000]="";
	strcat(info, path);
	strcat(info, "|");
	strcat(info, server_host);
	int i;
	for (i=0;i<12;i++) if (client_hosts[i] != NULL){
		char index[3];
		sprintf(index, "%d", i);
		strcat(info, "|");
		strcat(info, index);
		strcat(info, "|");
		strcat(info, client_hosts[i]);
	}

	//printw("%s\n", info);

	write(newfd, info, strlen(info));
	return 0;
}


/*
 * This function use select() to handle multi-client connection
 * Input: server socket descriptor
 * Return value:
 */
int run_server(int serv_socket){
	//Init ncurses screen

	initscr();
	printw("Listening...\n");
	refresh();

	// get the current path
	char path[50];
	getcwd(path, sizeof(path));

	// get localhost name
	char server_host[256];
	gethostname(server_host, sizeof(server_host));

	// store list of clients being connected
	// the socket descriptor is index of this array
	char* client_hosts[12];
	int i;
	for (i=0;i<12;i++) client_hosts[i] = NULL;

	// (graphics_function.c) print information about current connection state
	//printInfo(server_host,client_hosts);

	int ctrl_sock_fd = -1; // socket descriptor value of the client which is controller
	int max_sock_fd = serv_socket;	//max of socket descriptor values, used to iterator
	int mark; //mark for new connection or one of connections exit

	fd_set fds, fds_init;
	FD_ZERO(&fds);
	FD_ZERO(&fds_init);
	FD_SET(serv_socket, &fds_init);

	while(1){
		if (max_sock_fd != serv_socket){
			printw("%s@ ",path);	// print the current path before each command is typed
			refresh();
		}

		fds = fds_init;

		// wait for clients
		if (select(max_sock_fd + 1, &fds, NULL, NULL, NULL) == -1) {
			perror("select");
			continue;
		}

		int i;
		for (i=0; i<=max_sock_fd; i++) if (FD_ISSET(i, &fds))
		{
			if (i == serv_socket){
				//accept new connect and get the client host
				char hostbuf[NI_MAXHOST];
				int newfd = acceptNewConnect(serv_socket, hostbuf);
				printw("\n%s connected\n",hostbuf);
				refresh();
				add_client_host(client_hosts, newfd, hostbuf);
				FD_SET(newfd, &fds_init);
				if (newfd > max_sock_fd) max_sock_fd = newfd;
				if (ctrl_sock_fd == -1){
					ctrl_sock_fd = newfd;
					write(newfd,"controller",strlen("controller")+1);
				}
				else
					write(newfd,"not",strlen("not")+1);

				send_connection_info(newfd, path, server_host, client_hosts);
				mark = newfd;

				//write(newfd, path, strlen(path));

				//write(newfd, server_host, strlen(server_host));
				//printw("wrote server host\n");
				//send_client_hosts(client_hosts, newfd);

				//clear();
			}
			else
			if (i == ctrl_sock_fd){
				int j = receive_and_run(&ctrl_sock_fd, serv_socket, max_sock_fd, &fds_init, path, client_hosts, &mark);
				if (j==1){
					return 1;
				}
			}
			else
			if (i != serv_socket && i != ctrl_sock_fd){
				//printw("Socket %d has closed\n", i);
				FD_CLR(i, &fds_init);
				client_hosts[i] = NULL;
				mark=i;
				clear();
				/*printInfo(server_host, client_hosts);
				refresh();*/
			}
			printInfo(server_host, client_hosts);
			refresh();
		}
	}

	//not reach
	refresh();
	close(serv_socket);
	endwin();
}
