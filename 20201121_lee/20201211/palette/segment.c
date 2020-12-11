#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "segment.h"

int seg_fd;

void init_segment(){
    if((seg_fd=open("/dev/segment",O_RDWR|O_SYNC)) < 0) {
        printf("FND open fail\n");
        exit(1);
    }
}

void segment_write(unsigned short *color){
	write(seg_fd, color, 4);
}

void close_segment(){
	close(seg_fd);
}
