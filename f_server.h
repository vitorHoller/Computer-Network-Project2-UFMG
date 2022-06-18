#pragma once

#include <stdlib.h>

void add_csock(int csock, int *csock_vector, unsigned int n, int *index);
int add_equip(char buf[BUFSZ], int *equip_vector, unsigned int n, int *index);
void handle_buf(char buf[BUFSZ], int *equip_vector, unsigned int n, int index);
void request_equipment(char buf[BUFSZ], int *equip_vector, int n);
void list_equipments(char buf[BUFSZ], int *equip_vector, unsigned int n, int index);
