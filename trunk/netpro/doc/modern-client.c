/*
 * Sample modern client
 *
 * Usage:
 * % ./modern-client host.example.com daytime
 *
 * Id: modern-client.c,v 1.12 2003/08/15 03:16:41 yoshfuji Exp
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char *host, *port;
	struct addrinfo hints, *ai0, *ai;
	int s;
	int gai;

	/* Check the arguments */
	if (argc != 3) {
		fprintf(stderr, "Usage: %s [host name] [port number]\n", argv[0]);
		exit(1);
	}
	
	host = argv[1];
	port = argv[2];

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

	/* Check if we've connected to the remote host */
	if (s == -1) {
		fprintf(stderr, "Cannot connect to %s port %s\n", 
			host != NULL ? host : "(null)", 
			port);
		exit(1);
	}

	/* main loop */
	while (1) {
		ssize_t cc;
		char buf[1024];
		
		/* read message from remote */
		cc = read(s, buf, sizeof(buf));
		if (cc == -1) {
			perror("read");
			close(s);
			exit(1);
		} else if (cc == 0) {
			break;
		}

		/* output the message */
		if (write(STDOUT_FILENO, buf, cc) == -1) {
			perror("write");
			close(s);
			exit(1);
		}
	}

	close(s);
	exit(0);
}
