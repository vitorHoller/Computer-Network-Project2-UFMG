#include "common.h"
#include "f_server.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 500
#define MAX 15
#define CLOSE "close connection"
#define LIMIT "Equipment limit exceeded"
#define SUCCESS "Success removal"


int equip_vector[MAX] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int csock_vector[MAX] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void usage(int argc, char **argv)
{
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

struct client_data
{
    int csock;
    struct sockaddr_storage storage;
};

void *client_thread(void *data)
{
    struct client_data *cdata = (struct client_data *)data;
    struct sockaddr *caddr = (struct sockaddr *)(&cdata->storage);

    char caddrstr[BUFSZ];
    addrtostr(caddr, caddrstr, BUFSZ);
    printf("[log] connection from %s\n", caddrstr);

    char buf[BUFSZ];
    memset(buf, 0, BUFSZ);
    char aux[BUFSZ];
    memset(aux, 0, BUFSZ);
    int index = 0;
    if (add_equip(buf, equip_vector, MAX, &index) >= 0) //add equipment on the global vector equip_vector[MAX]
    {
        add_csock(cdata->csock, csock_vector, MAX, &index); // also add the cdata->csock at the global vector csock_vector[MAX], with the same index as equip_vector[MAX] (for broadcast)
        send(cdata->csock, buf, strlen(buf), 0);
        memset(buf, 0, BUFSZ);
    }
    else // if there are more than 15 equipments on the global vector equip_vector limits exceeds and close connection
    {
        send(cdata->csock, LIMIT, strlen(LIMIT), 0);
        close(cdata->csock);
        pthread_exit(EXIT_SUCCESS);
    }

    while (1)
    {
        recv(cdata->csock, buf, BUFSZ - 1, 0);
        strcpy(aux, buf);
        memset(buf, 0, BUFSZ);
        strncpy(buf, aux, strlen(aux) - 1);
        // printf("%ld bytes\n", strlen(buf));
        if (strcmp(buf, CLOSE) == 0)
        {
            memset(buf, 0, BUFSZ);
            sprintf(buf, SUCCESS);
            equip_vector[index - 1] = 0;
            csock_vector[index - 1] = 0;
            index = 0;
            send(cdata->csock, buf, strlen(buf), 0);
            close(cdata->csock);
            pthread_exit(EXIT_SUCCESS);
        }
        else
        {
            handle_buf(buf, equip_vector, MAX, index);
            send(cdata->csock, buf, strlen(buf), 0);
            memset(buf, 0, BUFSZ);
            memset(aux, 0, BUFSZ);
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

    while (1)
    {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1)
        {
            logexit("accept");
        }

        struct client_data *cdata = malloc(sizeof(*cdata));
        if (!cdata)
        {
            logexit("malloc");
        }
        cdata->csock = csock;
        memcpy(&(cdata->storage), &cstorage, sizeof(cstorage));

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, cdata);
    }

    exit(EXIT_SUCCESS);
}
