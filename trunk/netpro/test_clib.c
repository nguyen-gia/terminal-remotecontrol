/*
 * test_clib.c
 *
 *  Created on: Dec 7, 2010
 *      Author: abcd
 */

#include <client_lib.h>

int main(){
	int s = create_client_socket("12345", "123");
	char buf[] = "qwerty1234";
	write(s, buf, sizeof(buf));
}
