/*
 * client.h
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
#ifndef CLIENT_H_
#define CLIENT_H_


#endif /* CLIENT_H_ */

int create_client_socket(char* server_port_number, char *server_ip_address);
int send_command_character(int ch, int i32ConnectFD);
int recieve_command_result(char *result, int i32ConnectFD);
int receive_connection_info(int client_socket, char *path, char* server_host, char *client_hosts[]);
int receive_connection_update(char* buffer,char* client_hosts[]);
int run_client(int client_socket);
int run_ctrl_client(int client_socket, char* firstpath,char* client_hosts[],char * server_host);
int run_normal_client(int client_socket,char* firstpath, char* client_hosts[], char* server_host);
