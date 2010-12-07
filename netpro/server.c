/*
 * server.c
 *
 *  Created on: Dec 3, 2010
 *  Author: PREDATOR TEAM
 */
#include "server_lib.h"

int create_server_socket(char *port_number)
{
	struct addrinfo hints, *ai;
	struct addrinfo *ai0;
	char *host = NULL;
	int error;

	fd_set fds_init, fds;

	/* initial address info structure */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	error = getaddrinfo(host, port, &hints, &ai0);
	if(error){
		fprintf(stderr, "getaddrinfo(): %s", gai_strerror(error));
		return -1;
	}

	/* initialize fd_set for select() */
	FD_ZERO(&fds_init);

	int s = -1;

	for(ai = ai0; ai; ai = ai->next){
#ifdef IPV6_V6ONLY
		int on = 1;
#endif
		s = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if(s == -1) continue;
		else return s;
	}

	return s;
}

