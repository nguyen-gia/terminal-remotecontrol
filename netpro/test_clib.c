/*
 * test_clib.c
 *
 *  Created on: Dec 7, 2010
 *      Author: abcd
 */

#include "client_lib.h"

int main(){
	int s = create_client_socket("12345", "133.27.73.77");

	if (s == -1){
		perror("*error connecting*");
		exit(1);
	}

	char buf[] = "qwerty1234";
	write(s, buf, sizeof(buf));
	read(s, buf, sizeof(buf));
	printf("receive from server: %s\n", buf);
}
