#include "common.h"
#include "f_server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 500
#define EMPTY_ID 0
#define ESTEIRA_ID 01
#define GUINDASTE_ID 02
#define PONTE_ROLANTE_ID 03
#define EMPILHADEIRA_ID 04
#define TEMPERATURA_ID 01
#define PRESSAO_ID 02
#define VELOCIDADE_ID 03
#define CORRENTE_ID 04
#define EQUIP_SIZE 4

void usage(int argc, char **argv)
{
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) // colocar o tratamento de '/0' e '/n' neste arquivo e nao no cliente, melhor a identacao, organizar algumas funcoes de meio de codigo, tipo a que mostra quantos bytes foram enviados, etc
{
    equipment equipments[4];
    equipments[0].id = ESTEIRA_ID;
    equipments[1].id = GUINDASTE_ID;
    equipments[2].id = PONTE_ROLANTE_ID;
    equipments[3].id = EMPILHADEIRA_ID;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            equipments[i].sensor_array[j].id = EMPTY_ID;
        }
    }
    if (argc < 3)
    {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage))
    {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1)
    {
        logexit("socket");
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
    {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage)))
    {
        logexit("bind");
    }

    if (0 != listen(s, 10))
    {
        logexit("listen");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);
    struct sockaddr_storage cstorage;
    struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
    socklen_t caddrlen = sizeof(cstorage);

    int csock = accept(s, caddr, &caddrlen);
    if (csock == -1)
    {
        logexit("accept");
    }
    char caddrstr[BUFSZ];
    addrtostr(caddr, caddrstr, BUFSZ);
    printf("[log] connection from %s\n", caddrstr);
    char buf[BUFSZ];
    memset(buf, 0, BUFSZ);
    char aux[BUFSZ];
	memset(aux, 0, BUFSZ);
    size_t countr = 0; // count bytes received
    size_t counts = 0; // count bytes sent

    while (1)
    {
        countr = recv(csock, buf, BUFSZ, 0);
        strcpy(aux, buf);
        strncpy(buf, aux, strlen(aux)-1); //eliminate '/0'
        printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)countr, buf);

        handle_buffer(buf, equipments, EQUIP_SIZE); // verify which function the client wants to call (add, read, remove, list)
        kill(buf, s);
        counts = send(csock, buf, strlen(buf), 0);
        if (counts != strlen(buf))
        {
            logexit("send");
        }
        memset(buf, 0, BUFSZ);
    }
    exit(EXIT_SUCCESS);
}