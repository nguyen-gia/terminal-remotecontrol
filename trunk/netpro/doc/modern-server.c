/*
 * Sample modern server (2)
 *
 * Usage:
 * % ./modern-server local-address local-port
 *
 * Id: modern-server.c,v 1.16 2002/12/29 15:40:25 yoshfuji Exp
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

static const char *message = "Hello, world!\n";

int main(int argc, char **argv)
{
	char *host, *port;
	struct addrinfo hints, *ai0, *ai;
	int gai;
	int socknum = 0, *socklist = NULL;
	int maxfd = -1;
	fd_set fds_init, fds;
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
		host = NULL;
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

	/* initialize fd_set for select() */
	FD_ZERO(&fds_init);

	/* listen on all available ports on this system */
	for (ai = ai0; ai; ai = ai->ai_next) {
		int s;
		int *newlist;
#ifdef IPV6_V6ONLY
		int on = 1;
#endif

		/* create a socket */
		s = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if (s == -1)
			continue;

#ifdef IPV6_V6ONLY
		/*
		 * Try to set IPV6_V6ONLY; since some systems (e.g. old
		 * Linux) do not support IPV6_V6ONLY, just ignore the error
		 * here.
		 */
		if (ai->ai_family == AF_INET6 &&
		    setsockopt(s, 
			       IPPROTO_IPV6, IPV6_V6ONLY, 
			       &on, sizeof(on)) == -1) {
			perror("setsockopt(IPV6_V6ONLY)");
			/* fall through */
		}
#endif

		/* bind and listen on local address/port */
		if (bind(s, ai->ai_addr, ai->ai_addrlen) == -1) {
			close(s);
			continue;
		}

		if (listen(s, 5) == -1) {
			close(s);
			continue;
		}

		if (s >= FD_SETSIZE || socknum >= MAX_SOCKNUM) {
			close(s);
			fprintf(stderr, "too many file/socket descriptors\n");
			break;
		}
		
		/* (re)allocate socket list */
		newlist = realloc(socklist, sizeof(int)*(socknum+1));
		if (newlist == NULL) {
			perror("realloc");
			close(s);
			break;	/* XXX: fatal? */
		}

		socklist = newlist;
		socklist[socknum++] = s;

		/* set one bit in fd_set for select() */
		FD_SET(s, &fds_init);

		if (maxfd < s)
			maxfd = s;
	}
	
	/* we no longer use address information */
	freeaddrinfo(ai0);

	/* in case we got no socket */
	if (socknum == 0) {
		fprintf(stderr, 
			"Cannot allocate any listen sockets on %s port %s\n", 
			host != NULL ? host : "(null)", 
			port);
		exit(1);
	}

	/* main loop */
	while (1) {
		int i;

		fds = fds_init;

		/* wait for clients */
		if (select(maxfd + 1, &fds, NULL, NULL, NULL) == -1) {
			perror("select");
			continue;
		}

		/* process readable sockets */
		for (i = 0; i < socknum; i++) {
			int sock = socklist[i];

			if (FD_ISSET(sock, &fds)) {
				int newfd;
				struct sockaddr_storage ss;
				socklen_t sslen;
				ssize_t cc;
				char hostbuf[NI_MAXHOST];
				int gni;

				/* accept one client */
				sslen = sizeof(ss);
				newfd = accept(sock, (struct sockaddr *)&ss, &sslen);
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

				/* write some text the the client */
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
		}
	}

	/* not reached */
	for (i = 0; i < socknum; i++)
		close(socklist[i]);
	free(socklist);

	exit(0);
}

