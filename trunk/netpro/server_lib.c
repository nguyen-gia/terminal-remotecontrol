/*
 * server_lib.c
 *
 *  Created on: Dec 3, 2010
 *  Author: PREDATOR TEAM
 */

#include "server_lib.h"

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
