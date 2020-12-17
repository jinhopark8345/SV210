#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include "dipsw.h"
#include "gpio.h"
#include "camera.h"
#include "touchlcd.h"
#include "imagepr.h"
#include "textlcd.h"
#include "keypad.h"

static int gpio_dev;

int init_gpio(){

	int Oflags;

    signal(SIGIO, save_sp);
	gpio_dev = open("/dev/gpiobutton",O_RDWR);
	if(gpio_dev < 0) {
		printf( "gpio device open ERROR!\n");
		return -1;
	}
    gpio_button = 0;

    /* printf("gpio button initialized!\n"); */
	/* printf("Please push the GPIO_0 port!\n"); */

    fcntl(gpio_dev, F_SETOWN, getpid());

    Oflags = fcntl(gpio_dev, F_GETFL);
    fcntl(gpio_dev, F_SETFL, Oflags | FASYNC);

    return 1;
}

void close_gpio(){
	close(gpio_dev);
}


void save_sp(){
    if(!(GPIO_ON > 0)){
        // do nothing
    }else {
        read(gpio_dev,&gpio_button,2);
        gpio_button = 1;
        printf("gpio_button : %d\n",gpio_button);
    }
    return;
}
