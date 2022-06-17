all:
	gcc -Wall -c common.c
	gcc -Wall -c f_server.c
	gcc -Wall client.c common.o -o client
	gcc -Wall server.c common.o f_server.o -o server
	gcc -Wall server-mt.c common.o f_server.o -lpthread -o server-mt

clean:
	rm common.o f_server.o client server server-mt 
