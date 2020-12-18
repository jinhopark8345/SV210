
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "dotmatrix.h"
#include "dipsw.h"

int dot_dev;

void init_dotmatrix(){
	dot_dev = open("/dev/dotmatrix", O_WRONLY);
	if(dot_dev == -1){
		printf("Dotmatrix device driver open error!");
	
	}
}

void dotmatrix_write(unsigned short count){

    if(!(DOTMATRIX_ON>0)){ // TEXTLCD off
        // do nothing
    } else{
        write(dot_dev, &count, 4);
    }
}


void close_dotmatrix(){
	close(dot_dev);
}
