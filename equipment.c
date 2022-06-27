#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX 15
#define CLOSE "close connection"
#define LIMIT "Equipment limit exceeded"
#define SUCCESS "Success removal"

int equip_vector[MAX] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int csock_vector[MAX] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

struct client_data
{
	int csock;
};

void usage(int argc, char **argv)
{
	printf("usage: %s <server IP> <server port>\n", argv[0]);
	printf("example: %s 127.0.0.1 51511\n", argv[0]);
	exit(EXIT_FAILURE);
}

void *client_thread(void *data)
{
	while (1)
	{
		char buf[BUFSZ];
		memset(buf, 0, BUFSZ);
		char aux[BUFSZ];
		memset(aux, 0, BUFSZ);
		struct client_data *cdata = (struct client_data *)data;
		recv(cdata->csock, buf, BUFSZ, 0);
		if (strcmp(buf, SUCCESS) == 0) // verifies if the server removed the equipment from the array
		{
			puts(buf);
			close(cdata->csock);
			exit(EXIT_SUCCESS);
		}
		if (strlen(buf) != 0) // just prints if has something in buffer
		{
			strcpy(aux, buf);
        	strncpy(buf, aux, strlen(aux)-1); //eliminate '/0'
			puts(buf);
		}
	}
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		usage(argc, argv);
	}

	struct sockaddr_storage storage;
	if (0 != addrparse(argv[1], argv[2], &storage))
	{
		usage(argc, argv);
	}

	int s;
	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if (s == -1)
	{
		logexit("socket");
	}
	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if (0 != connect(s, addr, sizeof(storage)))
	{
		logexit("connect");
	}

	char addrstr[BUFSZ];
	addrtostr(addr, addrstr, BUFSZ);

	char buf[BUFSZ];
	char aux[BUFSZ];
	memset(buf, 0, BUFSZ);
	memset(aux, 0, BUFSZ);
	size_t counts = 0; // count bytes sent

	recv(s, buf, BUFSZ, 0);
	if (strcmp(buf, LIMIT) == 0)
	{
		puts(buf);
		close(s);
		exit(EXIT_SUCCESS);
	}

	puts(buf);

	while (1)
	{
		struct client_data *cdata = malloc(sizeof(*cdata));
		if (!cdata)
		{
			logexit("malloc");
		}
		cdata->csock = s;

		pthread_t tid;
		pthread_create(&tid, NULL, client_thread, cdata); // calls a thread that will be running recv() until the client asks to close connection
		memset(buf, 0, BUFSZ);
		memset(aux, 0, BUFSZ);
		fgets(buf, BUFSZ - 1, stdin); // get a string from the client
		counts = send(s, buf, strlen(buf), 0); // sends the buffer to the server
		if (counts != strlen(buf))
		{
			logexit("send");
		}
		memset(buf, 0, BUFSZ);
	}
	close(s);

	exit(EXIT_SUCCESS);
}