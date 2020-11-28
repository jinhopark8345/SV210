#include "dotmatrix.h"

int dot_dev;


void init_dotmatrix(){

	dot_dev = open("/dev/dotmatrix", O_WRONLY);
	if(dot_dev == -1){
		printf("Dotmatrix device driver open error!");
	
	}

}

void dotmatrix_write(int count){

	write(dot_dev, &count, 4);

}


void close_dotmatrix(){

	close(dot_dev);
}
