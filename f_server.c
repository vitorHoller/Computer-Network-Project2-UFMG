#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 500
#define MAX 15
#define TARGET "Target equipment not found"
#define LIST "list equipment"
#define REQUEST "request information from"
#define REQUEST_RESPONSE "requested information"
#define CLOSE "close connection"
#define LIMIT "Equipment limit exceeded"
#define SUCCESS "Success removal"
#define EMPTY "Empty list "

int add_equip(char buf[BUFSZ], int *equip_vector, unsigned int n, int *index, int *count) // return -1 if equip_vector[MAX] is full and 1 if it is not
{
    int i;
    char aux[BUFSZ];
    memset(aux, 0, BUFSZ);
    for (i = 0; i < n; i++) // run all equip_vector[MAX] values and when the equipment doesn't exist equip_vector[index] = 0
    {
        if (equip_vector[i] == 0) // when the first equip_vector[index] = 0 is found, we add the index value into equip_vector[index]
        {
            count[0]++;
            equip_vector[i] = count[0];
            *index = i + 1;
            memset(buf, 0, BUFSZ);
            sprintf(aux, "Equipment %.2d added", equip_vector[i]);
            puts(aux); // prints it in server terminal

            sprintf(buf, "New ID: %.2d", equip_vector[i]); // sends buf to send(buf, ...)
            return 1;
        }
    }
    return -1;
}

void list_equipments(char buf[BUFSZ], int *equip_vector, int *csock_vector, unsigned int n, int index)
{
    memset(buf, 0, BUFSZ);
    int numero[MAX] = {0};
    int aux1;
    char aux[BUFSZ];
    memset(aux, 0, BUFSZ);

    for (int i = 0; i < n; i++) // will be used to order the equip vector because it must be printed in order
    {
        if (i != index - 1)
        {
            numero[i] = equip_vector[i];
        }
    }

    for (int i = 0; i < n; i++) // order the aux vector in crescent order
    {
        for (int j = i + 1; j < n; j++)
        {
            if (numero[i] > numero[j])
            {
                aux1 = numero[i];
                numero[i] = numero[j];
                numero[j] = aux1;
            }
        }
    }

    for (int i = 0; i < n; i++)
    {
        if (numero[i] != 0 && i != index - 1)
        {
            memset(aux, 0, BUFSZ);
            sprintf(aux, "%.2d ", numero[i]); // prints the equipments in crescent order
            strcat(buf, aux);                 // concat all existed equipments excludind the equipment that sent the command
        }
    }

    if (strlen(buf) == 0)
    {
        sprintf(buf, EMPTY); // if there is just one equipment connected in the server and this equipment send
    }                        // "list equipment" command, it will prints in equipment's terminal this message

    memset(aux, 0, BUFSZ);
    strncpy(aux, buf, strlen(buf) - 1); // removes the blank space in the end of the string
    memset(buf, 0, BUFSZ);
    strcpy(buf, aux);
    send(csock_vector[index - 1], buf, strlen(buf), 0);
}

void request_equipment(char buf[BUFSZ], int *equip_vector, int *csock_vector, unsigned int n, long int n_index, int index)
{
    char aux[BUFSZ];
    memset(aux, 0, BUFSZ);
    memset(buf, 0, BUFSZ);

    for (int i = 0; i < n; i++)
    {
        if (equip_vector[i] == n_index && equip_vector[i] != 0)
        {
            memset(buf, 0, BUFSZ);
            sprintf(buf, "Value from %.2d: %.2f", equip_vector[i], ((float)rand() / (float)(RAND_MAX)) * 10);
            send(csock_vector[index - 1], buf, strlen(buf), 0); // sends a message to the terminal that requested an information
            memset(aux, 0, BUFSZ);
            sprintf(aux, REQUEST_RESPONSE);
            send(csock_vector[i], aux, strlen(aux), 0); // sends a message to the terminal that had some information requested

            return;
        }
    }

    sprintf(aux, "Equipment %.2ld not found", n_index);
    puts(aux);            // prints into server terminal
    sprintf(buf, TARGET); // sends it to send(buf, ...)
    send(csock_vector[index - 1], buf, strlen(buf), 0);
}

void close_connection(char buf[BUFSZ], int *equip_vector, int *csock_vector, unsigned int n, int index) // closes the connection between server and equipment
{
    char aux[BUFSZ];
    int s = csock_vector[index - 1];

    if (strcmp(buf, CLOSE) == 0) // verify if the client sent close connection command
    {
        memset(buf, 0, BUFSZ);
        memset(aux, 0, BUFSZ);

        sprintf(aux, "Equipment %.2d removed", equip_vector[index - 1]);
        puts(aux);             // prints the message in server terminal
        sprintf(buf, SUCCESS); // buf content will be sent to equipment using send()

        for (int i = 0; i < MAX; i++)
        {
            if (csock_vector[i] != 0 && i == index - 1)
            {
                send(csock_vector[index - 1], buf, strlen(buf), 0);
            }
            else if (csock_vector[i] != 0 && i != index - 1)
            {
                send(csock_vector[i], aux, strlen(aux), 0);
            }
        }
        close(s);
        // zero those vectors of equipment and csock
        equip_vector[index - 1] = 0; // index is i + 1 in add_equip()/add_cscock(), so equip_vector[i] == equip_vector[index-1]
        csock_vector[index - 1] = 0;
    }
}

int handle_buf(char buf[BUFSZ], int *equip_vector, int *csock_vector, unsigned int n, int index) // return -1 if an equipment asks to close connection
{
    if (strncmp(buf, LIST, strlen(LIST)) == 0)
    {
        list_equipments(buf, equip_vector, csock_vector, n, index);
        return 1;
    }
    else if (strncmp(buf, REQUEST, strlen(REQUEST)) == 0)
    {
        long int n_index = 0;
        char aux[BUFSZ];
        char *ptr[BUFSZ];
        *ptr = NULL;
        memset(aux, 0, BUFSZ);
        strcpy(aux, strrchr(buf, ' '));
        int count = 0;

        // Traverse the given string. If current character
        // is not space, then place it at index 'count++'
        for (int i = 0; i < strlen(aux); i++)
            if (aux[i] != ' ')
                aux[count++] = aux[i];
        aux[count] = '\0';

        n_index = atoi(aux); // get the id string that had some information requested converted to integer
        // get the string handled and it sends the index of the equipment that the client wants to know about
        request_equipment(buf, equip_vector, csock_vector, n, n_index, index);
        return 1;
    }
    else if (strncmp(buf, CLOSE, strlen(CLOSE)) == 0)
    {
        close_connection(buf, equip_vector, csock_vector, n, index);
        return -1;
    }
    return -1;
}
