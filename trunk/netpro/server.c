/*
 * server.c
 *
 *  Created on: Dec 3, 2010
 *  Author: PREDATOR TEAM
 */
#include "server_lib.h"

int main(int argc, char *argv[]){
	if(argc != 2){
		fprintf(stderr, "usage: %s [port-number]\n", argv[0]);
		exit(1);
	}

	// create an socket and bind to host with specify port
	int serv_socket = create_server_socket(argv[1]);
	if(serv_socket < 0){
		fprintf(stderr, "Error while creating socket\n");
		exit(2);
	}

	run_server(serv_socket);
}

