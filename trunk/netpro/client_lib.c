/*
 * client_lib.c
 *
 *  Created on: Dec 3, 2010
 *  Author: PREDATOR TEAM
 */

#include <client_lib.h>

int create_client_socket(char* server_port_number, char *server_ip_address){
	char *host, *port;
	struct addrinfo hints, *ai0, *ai;
	int s;
	int gai;

	host = server_ip_address;
	port = server_port_number;

	/* Resolve the addresses associated */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = 0;

	gai = getaddrinfo(host, port, &hints, &ai0);
	if (gai) {
		fprintf(stderr,
				"getaddrinfo(): %s port %s: %s\n",
				host, port, gai_strerror(gai));
		exit(1);
	}

	/* Loop for creating an active connection to the remote host */
	s = -1;
	for (ai = ai0; ai; ai = ai->ai_next) {
		s = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if (s == -1)
			continue;

		if (connect(s, ai->ai_addr, ai->ai_addrlen) == 0)
			break;

		close(s);
		s = -1;
	}

	/* release memory */
	freeaddrinfo(ai0);

	return s;
}
