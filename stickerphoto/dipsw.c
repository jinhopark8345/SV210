

#include "dipsw.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define bitcheck(byte, nbit) ((byte) & (1 << nbit))
#define KEYPAD_MASK 0
#define DOTMATRIX_MASK 1
#define TOUCHLCD_MASK 2
#define TEXTLCD_MASK 3
#define SEGMENT_MASK 4
#define GPIO_MASK 5

int init_dipsw() {
  if ((dip_dev = open("/dev/dipsw", O_RDONLY)) < 0) {
    perror("DIPSW open fail\n");
    return -1;
  }

  KEYPAD_ON = -1;
  DOTMATRIX_ON = -1;
  TOUCHLCD_ON = -1;
  TEXTLCD_ON = -1;
  SEGMENT_ON = -1;
  GPIO_ON = -1;

  return;
}

void dip_read() {
  int temp = -1;
  int temp2 = -1;
  int temp3 = -1;
  int temp4 = -1;
  int temp5 = -1;
  int temp6 = -1;
  read(dip_dev, &vkey, 4);

  KEYPAD_ON = bitcheck(vkey[1], KEYPAD_MASK);
  DOTMATRIX_ON = bitcheck(vkey[1], DOTMATRIX_MASK);
  TOUCHLCD_ON = bitcheck(vkey[1], TOUCHLCD_MASK);
  TEXTLCD_ON = bitcheck(vkey[1], TEXTLCD_MASK);
  SEGMENT_ON = bitcheck(vkey[1], SEGMENT_MASK);
  GPIO_ON = bitcheck(vkey[1], GPIO_MASK);
}

void close_dipsw() {
  close(dip_dev);
  return;
}
