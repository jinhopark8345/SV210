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
    IplImage *temp_cv_image = cvCreateImage(cvSize(LCD_WIDTH, LCD_HEIGHT), IPL_DEPTH_8U, 3);


    if(!(GPIO_ON > 0)){
        // do nothing
    }else {
        /* read(gpio_dev,&gpio_button,2); */
        /* gpio_button += 1; */
        /* printf("%d\n",gpio_button); */
        printf("gpio button pressed: save image as '%s'\n", SAVE_FILE);

        textlcd_write(SP_SAVE, 0, 0);

        /* if(DOTMATRIX_ON > 0){ */
        /*     dotmatrix_write(3); */
        /*     dotmatrix_write(2); */
        /*     dotmatrix_write(1); */
        /* } */
        // save whole lcd
        RGB2cvIMG(temp_cv_image, csframe, LCD_WIDTH, LCD_HEIGHT);

        // save image as cv format
        cvSaveImage(SAVE_FILE, temp_cv_image);

        cvReleaseImage(&temp_cv_image);
    }
    return;
}
