/*
 * server_lib.h
 *
 *  Created on: Dec 3, 2010
 *  Author: PREDATOR TEAM
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>

#ifndef MAX_SOCKNUM
# define MAX_SOCKNUM FD_SETSIZE


#endif

#ifndef SERVER_LIB_H_
#define SERVER_LIB_H_


#endif /* SERVER_LIB_H_ */

int create_server_socket(char *port_number);
int receive_command_name(char* cmd, int i32ConnectFD, fd_set*init, int maxfd, int serv_socket);
int get_command_result(char *res, char *cmd);
int send_command_result(char *result, int i32ConnectFD);
int parse(char *cmd, char *rtcargv[]);
int acceptNewConnect(int serv_socket, char *hostbuf);
void add_client_host(char *client_hosts[], int newfd, char* hostbuf);
int run_server(int serv_socket);
int receive_and_run(int* ctrl_sock_fd, int serv_socket, int max_sock_fd, fd_set* fds_init_addr, char*path, char* client_hosts[], int* mark);
int send_client_hosts(char* client_hosts[], int newfd);
int send_connection_info(int newfd, char *path, char *server_host, char *client_hosts[]);
int run_server(int serv_socket);

