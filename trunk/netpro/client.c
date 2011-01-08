/*
 * client.c
 *
 *  Created on: Dec 3, 2010
 *  Author: PREDATOR TEAM
 */

#include "client_lib.h"

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "usage: %s [ip-address] [port-number]\n", argv[0]);
		exit(0);
	}

	int client_socket = create_client_socket(argv[2], argv[1]);
	if (client_socket < 0) {
		perror("socket()");
		exit(1);
	}

	run_ctrl_client(client_socket);
}
