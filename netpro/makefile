CLIENT_LIB = client_lib.a
SERV_LIB = server_lib.a
GRAPHIC_LIB = graphics_functions.a

client : client.c $(CLIENT_LIB) $(GRAPHIC_LIB)
	gcc -g -o client client.c client_lib.a graphics_functions.a -lncurses

client.o : client.c
	gcc -c client.c

client_lib.a : client_lib.o
	ar r client_lib.a client_lib.o

client_lib.o : client_lib.c
	gcc -c client_lib.c

graphics_functions.a : graphics_functions.o
	ar r graphics_functions.a graphics_functions.o

graphics_functions.o : graphics_functions.c
	gcc -c graphics_functions.c

server : server.c $(SERV_LIB) $(GRAPHIC_LIB)
	gcc -g -o server server.c server_lib.a graphics_functions.a -lncurses

server_lib.a : server_lib.o
	ar r server_lib.a server_lib.o

server_lib.o : server_lib.c
	gcc -c server_lib.c