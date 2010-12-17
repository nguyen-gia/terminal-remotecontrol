/*
 * test_slib.c
 *
 *  Created on: Dec 7, 2010
 *      Author: abcd
 */

#include "server_lib.h"

int main(){
	/* main loop */
	int s = create_server_socket("12345");
	char *message = "Hello World\n";

	if (s < 0){
		printf("error while creating server socket\n");
		exit(0);
	}


	while (1) {
		int i;
		int newfd;
		struct sockaddr_storage ss;
		socklen_t sslen;
		ssize_t cc;
		char hostbuf[NI_MAXHOST];
		int gni;

		printf("Listening...\n");

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
}
