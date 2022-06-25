all:
	gcc -Wall -c common.c
	gcc -Wall -c f_server.c
	gcc -Wall client.c common.o -o client
	gcc -Wall client-mt.c common.o -lpthread -o client-mt
	gcc -Wall server-mt.c common.o f_server.o -lpthread -o server-mt

clean:
	rm common.o f_server.o client client-mt server-mt 
