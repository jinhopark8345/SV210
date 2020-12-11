#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>

#ifndef DIPSW_H
#define DIPSW_H

int dip_dev;
unsigned short vkey[2];

void init_dipsw();
void dip_read();
void close_dipsw();


// int kbhit();
// void read_gpio();


#endif
