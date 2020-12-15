
#ifndef DIPSW_H
#define DIPSW_H

int dip_dev;
unsigned short vkey[2];

void init_dipsw();
void dip_read();
void close_dipsw();


#endif
