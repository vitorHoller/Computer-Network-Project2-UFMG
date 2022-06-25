#pragma once

#include <stdlib.h>

int add_equip(char buf[BUFSZ], int *csock_vector, unsigned int n, int *index, int *count);
int handle_buf(char buf[BUFSZ], int *equip_vector, int *csock_vector, unsigned int n, int index);
void close_connection(char buf[BUFSZ], int *equip_vector, int *csock_vector, unsigned int n, int index);
void request_equipment(char buf[BUFSZ], int *equip_vector, int *csock_vector, unsigned int n, long int n_index, int index);
void list_equipments(char buf[BUFSZ], int *equip_vector, int *csock_vector, unsigned int n, int index);
