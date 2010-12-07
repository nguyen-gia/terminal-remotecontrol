/*
 * server_lib.h
 *
 *  Created on: Dec 3, 2010
 *  Author: PREDATOR TEAM
 */
#include <stdio.h>

#ifndef SERVER_LIB_H_
#define SERVER_LIB_H_


#endif /* SERVER_LIB_H_ */

int create_server_socket(int port_number);
int get_command_result(char *result, char *command);
int send_command_result(char *result, int i32ConnectFD);
char recieve_command_character(int i32ConnectFD);

