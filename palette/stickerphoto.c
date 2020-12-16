#include <fcntl.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h> // mmap PROT_
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "stickerphoto.h"
#include "keyboard.h"
#include "touchlcd.h"
#include "camera.h"
#include "imagepr.h"
#include "textlcd.h"
#include "segment.h"
#include "dotmatrix.h"
#include "gpio.h"
#include "keypad.h"
#include "dipsw.h"


#define KEYBOARD_WAITING 0
#define DIPSW_ON 255

unsigned short user_input=KEYBOARD_WAITING;
int keypad_count=0;

typedef struct set_color{
	unsigned short rgbcol;
	unsigned short red;
	unsigned short blue;
	unsigned short green;	
}set_color;
struct set_color col={30, 30, 30, 30};


unsigned short get_color(unsigned short brush_color){
    unsigned short tmp=0;
    switch (brush_color) {
    case SP_BRUSH_WHITE:
        brush_color = 0b1111111111111111;
        printf("brush color: white\n");
        break;
    case SP_BRUSH_RED:
        // brush_color = (0b11111 << 11);
        col.red += 6;//0b11111/10  10 steps for 7 segment
        if(col.red>=63){
            col.red = 0;
        }
        tmp = col.red;
        col.rgbcol = (col.red/6)*10000 + col.green/6*100 + col.blue/6;
        brush_color = tmp<<10;
        printf("brush color: red, color level: %d\n", col.rgbcol);
        break;
    case SP_BRUSH_BLUE:
        // brush_color = (0b11111);
        col.blue += 6;//0b111111/10  10 steps for 7 segment
        if(col.blue>=63){
            col.blue = 0;
        }
        tmp = col.blue;
        col.rgbcol = (col.red/6)*10000 + col.green/6*100 + col.blue/6;
        brush_color = tmp;
        printf("brush color: blue, color level: %d\n ", col.rgbcol);
        break;
    case SP_BRUSH_GREEN:
        // brush_color = (0b111111 << 5);
        col.green += 6;//0b111111/10  10 steps for 7 segment
        if(col.green>=63){
            col.green = 0;
        }
        tmp = col.green;
        col.rgbcol = (col.red/6)*10000 + col.green/6*100 + col.blue/6;
        brush_color = tmp<<5;
        printf("brush color: green, color level: %d\n", col.rgbcol);
        break;
    case SP_BRUSH_ERASER: // eraser
        brush_color = PALETTE_ERASER;
        printf("brush color: eraser\n");
        break;
    default:
        break;
    }
    
    return brush_color;
}



void init_sp_camera(unsigned char *fb_mapped){

    unsigned short user_input_tmp = 0;
    user_input = SP_CAMERA;
    while(user_input == SP_CAMERA){

        user_input_tmp = read_keypad();
        if(Ok_flag == 1){
            if(vkey[1] == DIPSW_ON){
                user_input = user_input_tmp;
            }else if(user_input_tmp == SP_EXIT){
                user_input = SP_EXIT;
            }else{
                user_input = SP_UNDEFINED_INPUT;
            }
        }

        // after read_camera2rgb, cis_rgb values changed
        // change csframe value -> show camera image on touchlcd
        read_camera2rgb();
        change_palette_image(cis_rgb);

        // display changed csframe values
        LCD_print(fb_mapped);

        if(user_input == SP_STOP){
            printf("Stop button pressed, start editing, use last saved image\n");

            // go back to original state
            user_input = SP_EDITMODE;
            break;
        }
    }
}


void init_sp_grayscale(unsigned char *fb_mapped){
    textlcd_write(user_input, 0,0);
    printf("y pressed: Gray scaling start\n");

    // update cis_rgb
    update_cis_rgb(cis_rgb);

    // start editing with the photo the user just took(last photo for n$
    init_grayscale(cis_rgb);

    // change csframe value -> show camera image on touchlcd
    change_palette_image(cis_rgb);

    // to show the user that saved image has been loaded
    LCD_print(fb_mapped);

    // go back to original state
    user_input = SP_EDITMODE;


    return;
}
void init_sp_facedetect(unsigned char *fb_mapped){

    /* facedetect variables */
    int num_detected_face = 0;
    unsigned short user_input_tmp = 0;
    IplImage *temp_cv_image = cvCreateImage(cvSize(320, 240), IPL_DEPTH_8U, 3);

    // start face detecting and after the app detects any face, start editing
    textlcd_write(user_input, 0,0);
    printf("f pressed: face detection start \n");
    printf("press q to quit\n");

    while(user_input != SP_STOP){

        // loop until only if user hasn't pressed 'S' or a face hasn't detected
        user_input_tmp = read_keypad();
        if(Ok_flag == 1){
            if(vkey[1] == DIPSW_ON){
                user_input = user_input_tmp;
            }else if(user_input_tmp == SP_EXIT){
                user_input = SP_EXIT;
            }else{
                user_input = SP_UNDEFINED_INPUT;
            }
        }

        // after read_camera2rgb, cis_rgb values changed
        read_camera2rgb();

        // change csframe value -> show camera image on touchlcd
        change_palette_image(cis_rgb);

        // display changed csframe values
        LCD_print(fb_mapped);

        // detect face based on cis_rgb
        num_detected_face = detect_face(temp_cv_image, cis_rgb, fb_mapped);
        printf("%d detected!!\n", num_detected_face);
        dotmatrix_write(num_detected_face);

        if(num_detected_face > 0){
            user_input = SP_STOP;
        }
    }
	cvReleaseImage(&temp_cv_image);

}





void init_stickerphoto(){

  /* palette variables */
  unsigned short white = 1;
  unsigned short red = 2;
  unsigned short blue = 3;
  unsigned short green = 4;
  unsigned short eraser = 0;
  unsigned short user_input_tmp = 0;
  unsigned short brush_color = green;
  unsigned short segment_flag =0;

  char* background_image = "background.bmp";

  IplImage *load_image = NULL;

  unsigned char *fb_mapped;
  unsigned short keypad_input = -1;
  unsigned short keypad_input_tmp = 0;
  int i;

  init_gpio();
  init_keypad();
  init_dipsw();
  init_touchlcd(); //
  init_keyboard();
  init_camera(); // camer device file : camera_fd
  init_facedetect(fb_mapped);
  init_textlcd();
  init_segment();
  init_dotmatrix();
  init_palette(background_image);   // prepare touchlcd

  fb_mapped = lcdvar.fb_mapped;


  while (user_input != SP_EXIT) { // KEYPAD44

    segment_write(&(col.rgbcol));
    dip_read();

    user_input_tmp = read_keypad();
    if(Ok_flag == 1){
        if(vkey[1] == DIPSW_ON){
            user_input = user_input_tmp;
        }else if(user_input_tmp == SP_EXIT){
            user_input = SP_EXIT;
        }else{
            user_input = SP_UNDEFINED_INPUT;
        }



        switch(user_input) {
        case SP_EXIT:
            printf("KEYPAD44 pressed: exit the program \n");
            break;
        case SP_BRUSH_WHITE:
        case SP_BRUSH_RED:
        case SP_BRUSH_GREEN:
        case SP_BRUSH_BLUE :
        case SP_BRUSH_ERASER:
            textlcd_write(user_input, 0, brush_color);
            brush_color = get_color(user_input);
            break;
        case SP_BRUSH_SIZEUP:
            printf("+ pressed \n");
            brush_size += BRUSH_STEP;
            printf("increase brush size, brush size: %d\n",brush_size);
            textlcd_write(user_input, brush_size,0);
            break;
        case SP_BRUSH_SIZEDOWN:
            printf("- pressed \n");
            brush_size -= BRUSH_STEP;
            printf("reduce brush size, brush size: %d\n", brush_size);
            textlcd_write(user_input, brush_size,0);
            break;

        /* case 's': */
        /*     printf("save current touchlcd image"); */
        /*     user_input = KEYBOARD_WAITING; */
        /*     break; */

        case SP_CAMERA:
            printf("c pressed, plain camera starts, save image with gpio button \n");
            printf("quit: press the same button(c) again \n");
            init_sp_camera(fb_mapped);
            break;

        case SP_FACEDETECTION:
            init_sp_facedetect(fb_mapped);
            break;

        case SP_GRAYSCALE:
            init_sp_grayscale(fb_mapped);
            break;

        case SP_LOAD_IMAGE:
            load_image = cvLoadImage(SAVE_FILE ,CV_LOAD_IMAGE_COLOR);
            if(load_image == NULL){
                printf("\nload image error!");
                return;
            }

            //cvIMG2RGB565(load_image, cis_rgb, PALETTE_IMAGE_WIDTH, PALETTE_IMAGE_HEIGHT);// for face_file load
            load_img2LCD(load_image);

            // load_image size is much bigger than cis_rgb, that's why it doesn't work
            /* cvIMG2RGB565(load_image, cis_rgb, LCD_WIDTH, LCD_HEIGHT); */

            // save image file rgb value to csframe for editing purpose
            /* change_palette_image(cis_rgb); */
            // to show the user that saved image has been loaded
            LCD_print(fb_mapped);

            break;
        case SP_UNDEFINED_INPUT:
            textlcd_write(user_input, 0,0);
            break;

        case SP_EDITMODE:
            // do nothing, editing mode
            break;

        default:
            /* printf("%c\n", user_input); */
            /* printf("undefined key pressed \n"); */
            break;

        }

    }


    if (GetTouch() != -1) {
      setFrame(x_detected, y_detected, brush_color, brush_size);
      LCD_print(fb_mapped);
    }
  }

  cvReleaseImage(&load_image);
  close_gpio();
  close_keypad();
  close_dipsw();
  close_keyboard();
  close_camera();
  close_LCD(); close_textlcd();
  close_dotmatrix();
  close_segment();
  close_facedetect();
}

int main(void) {
    init_stickerphoto();
    return 0;
}
