#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define SUCCESS "Success removal"
#define LIMIT "Equipment limit exceeded"


void usage(int argc, char **argv)
{
	printf("usage: %s <server IP> <server port>\n", argv[0]);
	printf("example: %s 127.0.0.1 51511\n", argv[0]);
	exit(EXIT_FAILURE);
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

	printf("connected to %s\n", addrstr);

	char buf[BUFSZ];
	char aux[BUFSZ];
	memset(buf, 0, BUFSZ);
	memset(aux, 0, BUFSZ);
	unsigned total = 0;
	size_t countr = 0; // count bytes received
	size_t counts = 0; // count bytes sent

	recv(s, buf, BUFSZ, 0);
	if(strcmp(buf, LIMIT) == 0){
		close(s);
		exit(EXIT_SUCCESS);
	}

	puts(buf);
	
	while (1)
	{
		memset(buf, 0, BUFSZ);
		memset(aux, 0, BUFSZ);
	    printf("mensagem> ");
		fgets(buf, BUFSZ - 1, stdin);
		counts = send(s, buf, strlen(buf), 0);
		if (counts != strlen(buf))
		{
			logexit("send");
		}
		memset(buf, 0, BUFSZ);
		countr = recv(s, buf, BUFSZ - total, 0);
		puts(buf);
		if(strcmp(buf, SUCCESS) == 0){
			close(s);
			exit(EXIT_SUCCESS);
		}
		total += countr; // total of bytes received during the communication
		printf("received %ld bytes and %u bytes in total\n", countr, total);
		strcpy(aux, buf);
        strncpy(buf, aux, strlen(aux)-1); //eliminate '/0'
		kill(buf, s);
	}
	close(s);

	exit(EXIT_SUCCESS);
}