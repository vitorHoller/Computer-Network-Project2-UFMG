#pragma once

#include <stdlib.h>

#define BUFSZ 500 // buffer length 
#define MAX 15

typedef struct sensor
{
    int id;
} sensor;

typedef struct equipment
{
    int id;
    sensor sensor_array[4];
} equipment;

void add(char sensors[BUFSZ], char equip[BUFSZ], char aux[BUFSZ], equipment *equipments, unsigned int n);
void _remove(char sensors[BUFSZ], char equip[BUFSZ], char aux[BUFSZ], equipment *equipments, unsigned int n);
void list(char equip[BUFSZ], char aux[BUFSZ], equipment *equipments, unsigned int n);
void _read(char sensors[BUFSZ], char equip[BUFSZ], char aux[BUFSZ], equipment *equipments, unsigned int n);

void handle_add(char aux[BUFSZ], equipment *equipments, unsigned int n);
void handle_remove(char aux[BUFSZ], equipment *equipments, unsigned int n);
void handle_list(char aux[BUFSZ], equipment *equipments, unsigned int n);
void handle_read(char aux[BUFSZ], equipment *equipments, unsigned int n);
void handle_buffer(char buf[BUFSZ], equipment *equipment, unsigned int n);

void add_csock(int csock, int *csock_vector, unsigned int n, int *index);
int add_equip(char buf[BUFSZ], int *equip_vector, unsigned int n, int *index);