#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSZ 500
#define MAX 15
#define TARGET "Target equipment not found"
#define LIST "list equipment"
#define REQUEST "request information from 0"


int add_equip(char buf[BUFSZ], int *equip_vector, unsigned int n, int *index) // return -1 if equip_vector[MAX] is full and 1 if it is not
{
    int i;
    char aux[BUFSZ];
    memset(aux, 0, BUFSZ);
    for (i = 0; i < n; i++) // run all equip_vector[MAX] values and when the equipment doesn't exist equip_vector[index] = 0
    {
        if (equip_vector[i] == 0) // when the first equip_vector[index] = 0 is found, we add the index value into equip_vector[index]
        {
            equip_vector[i] = i + 1;
            *index = i + 1;
            memset(buf, 0, BUFSZ);
            sprintf(aux, "Equipment 0%d added", equip_vector[i]); 
            puts(aux); // prints it in server terminal
            sprintf(buf, "New ID: 0%d", equip_vector[i]); // sends buf to send(buf, ...)
            return 1;
        }
    }
    return -1;
}

void add_csock(int csock, int *csock_vector, unsigned int n, int *index) // same as add_equip but for csock
{
    int i;
    for (i = 0; i < n; i++)
    {
        if (csock_vector[i] == 0)
        {
            csock_vector[i] = csock;
            *index = i + 1;
            return;
        }
    }
}

void list_equipments(char buf[BUFSZ], int *equip_vector, unsigned int n, int index)
{
    memset(buf, 0, BUFSZ);
    char aux[BUFSZ];
    memset(aux, 0, BUFSZ);
    for (int i = 0; i < n; i++)
    {
        if (equip_vector[i] != 0 && equip_vector[i] != index)
        {
            memset(aux, 0, BUFSZ);
            sprintf(aux, "0%d ", equip_vector[i]);
            strcat(buf, aux); // concat all existed equipments excludind the equipment that sent the command
        }
    }
    memset(aux, 0, BUFSZ);
    strncpy(aux, buf, strlen(buf) - 1); // removes the blank space in the end of the string
    memset(buf, 0, BUFSZ);
    strcpy(buf, aux);
}

void request_equipment(char buf[BUFSZ], int *equip_vector, int index)
{
    char aux[BUFSZ];
    memset(aux, 0, BUFSZ);
    memset(buf, 0, BUFSZ);
    if (equip_vector[index - 1] == index) // get information of the index that the client wants
    {
        sprintf(buf, "Value from 0%d: %.2f", equip_vector[index - 1], 2.56);
    }
    else
    {
        sprintf(aux, "Equipment 0%d not found", index);
        puts(aux); // prints into server terminal
        sprintf(buf, TARGET); // sends it to send(buf, ...)
    }
}

void handle_buf(char buf[BUFSZ], int *equip_vector, unsigned int n, int index)
{
    if (strncmp(buf, LIST, strlen(LIST)) == 0)
    {
        list_equipments(buf, equip_vector, n, index);
    }
    else if (strncmp(buf, REQUEST, strlen(REQUEST)) == 0)
    {
        long int n_index = 0;
        char aux[BUFSZ];
        char *ptr[BUFSZ];
        *ptr = NULL;
        memset(aux, 0, BUFSZ);
        strcpy(aux, strrchr(buf, '0'));
        n_index = strtol(aux, ptr, 0);
        //get the string handled and it sends the index of the equipment that the client wants to know about
        request_equipment(buf, equip_vector, n_index);
    }
}
