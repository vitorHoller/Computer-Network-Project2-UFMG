all:
	gcc -Wall -c common.c
	gcc -Wall -c f_server.c
	gcc -Wall client.c common.o -o client
	gcc -Wall equipment.c common.o -lpthread -o equipment
	gcc -Wall server.c common.o f_server.o -lpthread -o server

clean:
	rm common.o f_server.o client equipment server 