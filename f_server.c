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
#define REQUEST "request information from 0"
#define REQUEST_RESPONSE "request information"
#define CLOSE "close connection"
#define LIMIT "Equipment limit exceeded"
#define SUCCESS "Success removal"

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
            printf("%d\n", count[0]);
            equip_vector[i] = count[0];
            *index = i + 1;
            memset(buf, 0, BUFSZ);
            sprintf(aux, "Equipment 0%d added", equip_vector[i]);
            puts(aux);                                    // prints it in server terminal
            sprintf(buf, "New ID: 0%d", equip_vector[i]); // sends buf to send(buf, ...)
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

    for (int i; i < n; i++) // will be used to order the equip vector because it must be printed in order
    {
        if (i != index - 1)
        {
            numero[i] = equip_vector[i];
        }
    }

    for (int i = 0; i < n; i++) // order the aux vector
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
            sprintf(aux, "0%d ", numero[i]); // prints the equipments in crescent order
            strcat(buf, aux);                // concat all existed equipments excludind the equipment that sent the command
        }
    }
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
    if (equip_vector[n_index - 1] != 0) // get information of the index that the client wants
    {
        if (equip_vector[n_index - 1] < 10)
        {
            sprintf(buf, "Value from 0%d: %.2f", equip_vector[n_index - 1], 2.56);
        }
        else
        {
            sprintf(buf, "Value from %d: %.2f", equip_vector[n_index - 1], 2.56);
        }
        send(csock_vector[index - 1], buf, strlen(buf), 0); // sends a message to the terminal that requested an information
        sprintf(aux, REQUEST_RESPONSE);
        send(csock_vector[n_index - 1], aux, strlen(aux), 0); // sends a message to the terminal that had some information requested
    }
    else
    {
        if (n_index < 10)
        {
            sprintf(aux, "Equipment 0%ld not found", n_index);
        }
        else
        {
            sprintf(aux, "Equipment %ld not found", n_index);
        }
        puts(aux);            // prints into server terminal
        sprintf(buf, TARGET); // sends it to send(buf, ...)
        send(csock_vector[index - 1], buf, strlen(buf), 0);
    }
}

void close_connection(char buf[BUFSZ], int *equip_vector, int *csock_vector, unsigned int n, int index)
{
    char aux[BUFSZ];
    int s = csock_vector[index - 1];
    if (strcmp(buf, CLOSE) == 0) // verify if the client sent close connection command
    {
        memset(buf, 0, BUFSZ);
        memset(aux, 0, BUFSZ);
        if (equip_vector[index - 1] < 10)
        {
            sprintf(aux, "Equipment 0%d removed", equip_vector[index - 1]);
        }
        else
        {
            sprintf(aux, "Equipment %d removed", equip_vector[index - 1]);
        }
        puts(aux);
        sprintf(buf, SUCCESS);

        for (int i = 0; i < MAX; i++)
        {
            if (csock_vector[i] != 0 && equip_vector[i] == index)
            {
                send(csock_vector[i], buf, strlen(buf), 0);
            }
            else if (csock_vector[i] != 0)
            {
                send(csock_vector[i], aux, strlen(aux), 0);
            }
        }
        close(s);
        // zero those vectors of equipment and csock
        equip_vector[index - 1] = 0; // index is i + 1 in add_equip()/add_cscock(), so equip_vector[i] == equip_vector[index-1]
        csock_vector[index - 1] = 0;
        index = 0; 
    }
}

int handle_buf(char buf[BUFSZ], int *equip_vector, int *csock_vector, unsigned int n, int index)
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
        for (int i = 0; aux[i]; i++)
            if (aux[i] != ' ')
                aux[count++] = aux[i];
        aux[count] = '\0';

        n_index = strtol(aux, ptr, 0);
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
