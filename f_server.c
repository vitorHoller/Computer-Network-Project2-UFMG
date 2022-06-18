#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSZ 500
#define MAX 15
#define TARGET "Target equipment not found"
#define LIST "list equipment"
#define REQUEST "request information from 0"
int add_equip(char buf[BUFSZ], int *equip_vector, unsigned int n, int *index)
{
    int i;
    for (i = 0; i < n; i++)
    {
        if (equip_vector[i] == 0)
        {
            equip_vector[i] = i + 1;
            *index = i + 1;
            memset(buf, 0, BUFSZ);
            sprintf(buf, "Equipment 0%d added", equip_vector[i]);
            return 0;
        }
    }
    return -1;
}

void add_csock(int csock, int *csock_vector, unsigned int n, int *index)
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
            strcat(buf, aux);
        }
    }
    memset(aux, 0, BUFSZ);
    strncpy(aux, buf, strlen(buf) - 1);
    memset(buf, 0, BUFSZ);
    strcpy(buf, aux);
}

void request_equipment(char buf[BUFSZ], int *equip_vector, int n)
{
    memset(buf, 0, BUFSZ);
    if (equip_vector[n - 1] == n)
    {
        sprintf(buf, "Value from 0%d: %.2f", equip_vector[n - 1], 3.22);
    }
    else
    {
        sprintf(buf, TARGET);
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
        request_equipment(buf, equip_vector, n_index);
    }
}
