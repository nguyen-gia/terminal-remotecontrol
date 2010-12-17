/*
 * test_slib.c
 *
 *  Created on: Dec 7, 2010
 *      Author: abcd
 */

#include "server_lib.h"
#include <ncurses.h>

int main(){
	/* main loop */
	int s = create_server_socket("12345");
	char *message = "Hello World\n";

	if (s < 0){
		printf("error while creating server socket\n");
		exit(0);
	}


	initscr();

	/*printw("abcdTesting...\n");
	refresh();
	sleep(2);*/

	while (1) {
		int i;
		int newfd;
		struct sockaddr_storage ss;
		socklen_t sslen;
		ssize_t cc;
		char hostbuf[NI_MAXHOST], buf[NI_MAXHOST + 2];
		int gni;
		char ch[1];

		printw("Listening...\n");
		refresh();

		//wait for client
		sslen = sizeof(ss);
		newfd = accept(s, (struct sockaddr *)&ss, &sslen);
		if (newfd == -1) {
			perror("accept");
			continue;
		}

		//print remote address
		gni = getnameinfo((struct sockaddr *)&ss, sslen,
				hostbuf, sizeof(hostbuf),
				NULL, 0,
				NI_NUMERICHOST);
		if (gni)
			strcpy(hostbuf, "???");	//XXX

		printw("%s@ ", hostbuf);
		refresh();

		while (1){
			read(newfd, ch, 1);
			printw("%s", ch);
			refresh();
			if (ch[0] == '\n') break;
		}

		refresh();
		// write some text to the client

		// done
		close(newfd);
	}

	endwin();

	/* not reached */
	//close(s);
}
