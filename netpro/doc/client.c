/*
 * client.c
 *
 *  Created on: Nov 12, 2010
 *      Author: abcd
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

int main(int args, char* argv[]){
	if (args < 3){
		printf("too few arguments !\n");
		exit(-1);
	}
	char* host = argv[1];
	char* serv = argv[2];

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	struct addrinfo* ai;
	struct addrinfo hints;


	int error;

	if (error = getaddrinfo(host, serv, NULL, &ai)) {
		fprintf(stderr, "getaddrinfo(%s, %s, ...): %s (%d)\n", host, serv, gai_strerror(error),
				error);
		exit(-1);
	}
	  /* loop through addrinfos until we get a good connect */
	for (; ai; ai = ai->ai_next) {
		printf("1\n");
	    if ( 0 == connect(sockfd, ai->ai_addr, ai->ai_addrlen)){
	    	break;
	    }
	    /* break; once you get one that works */

	}

	char buf[100];

	while(1){
		if (-1 == recv(sockfd, buf, sizeof(buf), 0)){
			printf("error recving\n");
			break;
		}
		printf("Server: %s", buf);

		scanf("%s", buf);
		send(sockfd, buf, sizeof(buf), 0);
	}

}
