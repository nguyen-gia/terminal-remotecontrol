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
int get_command_name(char *cmd, int i32ConnectFD);
int get_command_result(char *res, char *cmd);
int send_command_result(char *result, int i32ConnectFD);
char recieve_command_character(int i32ConnectFD);

