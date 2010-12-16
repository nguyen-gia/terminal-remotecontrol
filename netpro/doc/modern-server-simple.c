/*
 * Sample modern server (1)
 *
 * Usage:
 * % ./modern-server [local-address] local-port
 *
 * Id: modern-server-simple.c,v 1.2 2004/07/24 15:34:58 yoshfuji Exp
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

static const char *message = "Hello, world!\n";

int main(int argc, char **argv)
{
	char *host, *port;
	struct addrinfo hints, *ai0, *ai;
	int gai;
	int s;
	int i;

	/* process the arguments */
	if (argc != 2 && argc != 3) {
		fprintf(stderr, "Usage: %s [host] portnum\n", argv[0]);
		exit(1);
	}

	if (argc == 3) {
		host = argv[1];
		port = argv[2];
	} else {
		host = NULL;	/* Unspecified address */
		port = argv[1];
	}

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
		int s;

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

	/* in case we failed to create a socket... */
	if (s == -1) {
		fprintf(stderr, 
			"Cannot allocate any listen sockets on %s port %s\n", 
			host != NULL ? host : "(null)", 
			port);
		exit(1);
	}

	/* main loop */
	while (1) {
		int i;
		int newfd;
		struct sockaddr_storage ss;
		socklen_t sslen;
		ssize_t cc;
		char hostbuf[NI_MAXHOST];
		int gni;

		/* wait for client */
		sslen = sizeof(ss);
		newfd = accept(s, (struct sockaddr *)&ss, &sslen);
		if (newfd == -1) {
			perror("accept");
			continue;
		}

		/* print remote address */
		gni = getnameinfo((struct sockaddr *)&ss, sslen,
				  hostbuf, sizeof(hostbuf),
				  NULL, 0,
				  NI_NUMERICHOST);
		if (gni)
			strcpy(hostbuf, "???");	/*XXX*/

		printf("accept from %s\n", hostbuf);

		/* write some text to the client */
		cc = write(newfd, message, strlen(message));
		if (cc == -1) {
			perror("write");
		} else if (cc != strlen(message)) {
			fprintf(stderr, 
				"write returned %d "
				"while %d is expected.\n",
				cc, strlen(message));
		}

		/* done */
		close(newfd);
	}

	/* not reached */
	close(s);

	exit(0);
}

