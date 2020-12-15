

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include "dipsw.h"

void init_dipsw(){
    if ((dip_dev=open("/dev/dipsw",O_RDONLY)) < 0) {
          perror("DIPSW open fail\n");
          return -1;
    }
    return ;
}

void dip_read(){
    read(dip_dev, &vkey, 4);
   // printf("%d %d\n", vkey[0], vkey[1]);
}


void close_dipsw(){
    close(dip_dev);
    return ;
}
