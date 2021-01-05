#ifndef DIPSW_H
#define DIPSW_H

int dip_dev;
unsigned short vkey[2];

int KEYPAD_ON;
int DOTMATRIX_ON;
int TOUCHLCD_ON;
int TEXTLCD_ON;
int SEGMENT_ON;
int GPIO_ON;

int init_dipsw();
void dip_read();
void close_dipsw();

#endif
