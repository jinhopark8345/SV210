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

#include "keyboard.h"
#include "touchlcd.h"
#include "camera.h"
#include "facedetect.h"
#include "textlcd.h"
#include "segment.h"
#include "dotmatrix.h"


#define KEYBOARD_WAITING 0
#define ESC 27

typedef struct set_color{
	unsigned short rgbcol;
	unsigned short red;
	unsigned short blue;
	unsigned short green;	
};

struct set_color col={30, 30, 30, 30};

unsigned short get_color(unsigned short brush_color){
    unsigned short tmp=0;
    printf("%c pressed\n", brush_color);
    switch (brush_color) {
    case 'w': 
        brush_color = 0b1111111111111111;
        printf("brush color: white\n");
        break;
    case 'r': // red
        // brush_color = (0b11111 << 11);
        col.red += 6;//0b11111/10  10 steps for 7 segment
        if(col.red>=63){
            col.red = 0;
        }
        tmp = col.red;
        col.rgbcol = (col.red/6)*10000 + col.green/6*100 + col.blue/6;
        printf("%d\n", col.rgbcol);
        brush_color = tmp<<10;
        printf("brush color: red\n");
        break;
    case 'b': // blue
        // brush_color = (0b11111);
        col.blue += 6;//0b111111/10  10 steps for 7 segment
        if(col.blue>=63){
            col.blue = 0;
        }
        tmp = col.blue;
        col.rgbcol = (col.red/6)*10000 + col.green/6*100 + col.blue/6;
        brush_color = tmp;
        printf("brush color: blue\n");
        break;
    case 'g': // green
        // brush_color = (0b111111 << 5);
        col.green += 6;//0b111111/10  10 steps for 7 segment
        if(col.green>=63){
            col.green = 0;
        }
        tmp = col.green;
        col.rgbcol = (col.red/6)*10000 + col.green/6*100 + col.blue/6;
        brush_color = tmp<<5;
        printf("brush color: green\n");
        break;
    case 'e': // eraser
        brush_color = PALETTE_ERASER;
        printf("brush color: eraser\n");
        break;
    default:
        break;
    }
    
    return brush_color;
}

int main(void) {
  /* facedetect variables */
  int num_detected_face = 0;

  /* palette variables */
  unsigned short white = 1;
  unsigned short red = 2;
  unsigned short blue = 3;
  unsigned short green = 4;
  unsigned short eraser = 0;
  unsigned short keyboard_input=KEYBOARD_WAITING;
  unsigned short keyboard_input_tmp = 0;
  unsigned short brush_color = green;
  unsigned short segment_flag =0; 
  unsigned short tmp_red=0, tmp_blue=0, tmp_green=0;

  IplImage *temp_cv_image = NULL;
  char* face_file = "face_image.jpg";
  char* background_image = "background.bmp";
  unsigned char *fb_mapped;


  init_touchlcd(); //
  init_keyboard();
  init_camera(); // camer device file : camera_fd
  init_facedetect(fb_mapped);
  init_textlcd();
  init_segment();
  init_dotmatrix();
  init_palette(background_image, face_file);   // prepare touchlcd

  fb_mapped = lcdvar.fb_mapped;
  temp_cv_image = cvCreateImage(cvSize(320, 240), IPL_DEPTH_8U, 3);

  while (keyboard_input != ESC) {//ESC
    segment_write(&(col.rgbcol));

    if(kbhit()){
        keyboard_input = readch();

        switch(keyboard_input) {  
	    case 'w':
            brush_color = get_color(keyboard_input);
            textlcd_write(keyboard_input, 0, brush_color);
            break;
        case 'r':
            brush_color = get_color(keyboard_input);
            textlcd_write(keyboard_input, 0, brush_color);
            break;
        case 'b':
            brush_color = get_color(keyboard_input);
            textlcd_write(keyboard_input, 0,brush_color);
            break;
        case 'g':
            brush_color = get_color(keyboard_input);
            textlcd_write(keyboard_input, 0, brush_color);
            break;
        case 'e':
            textlcd_write(keyboard_input, 0, brush_color);
            brush_color = get_color(keyboard_input);
            break;
        case '+':
            printf("+ pressed \n");
            brush_size += BRUSH_STEP;
            printf("increase brush size, brush size: %d\n",brush_size);
            textlcd_write(keyboard_input, brush_size,0);
            break;
        case '_':
            printf("- pressed \n");
            brush_size -= BRUSH_STEP;
            printf("reduce brush size, brush size: %d\n", brush_size);
            textlcd_write(keyboard_input, brush_size,0);
            break;

            // it's not necessary to start using app(stickerphoto) with face-detect, most of the users will want to take photo as they want and edit the image


        case 's':
            printf("save current touchlcd image");
            keyboard_input = KEYBOARD_WAITING;

            break;

        case 'c':
            printf("c pressed, plain camera starts, save image with 's' \n");
            while(keyboard_input != 'q'){

                // after read_camera2rgb, cis_rgb values changed
                // change csframe value -> show camera image on touchlcd
                read_camera2rgb();
                change_palette_image(cis_rgb);

                // display changed csframe values
                LCD_print(fb_mapped);

                if(kbhit()){
                    keyboard_input = readch();
                }

                if(keyboard_input == 's'){
                    RGB2cvIMG(temp_cv_image, cis_rgb, PALETTE_IMAGE_WIDTH, PALETTE_IMAGE_HEIGHT);

                    // save image as cv format
                    cvSaveImage(face_file, temp_cv_image);
                    printf("s pressed, save image, saved file name: %s\n", face_file);

                    // go back to original state
                    keyboard_input = 'c';
                }

                if(keyboard_input == 'S'){
                    printf("S pressed, start editing, use last saved image\n");

                    // start editing with the photo the user just took(last photo for now)

                    // from cvImg format to rgb format for displaying, currently since I am not opening face_file, it can only work with the last captured image
                    cvIMG2RGB565(temp_cv_image, cis_rgb, PALETTE_IMAGE_WIDTH, PALETTE_IMAGE_HEIGHT);

                    // save face_file rgb value to csframe for editing purpose
                    change_palette_image(cis_rgb);

                    // to show the user that saved image has been loaded
                    LCD_print(fb_mapped);

                    // go back to original state
                    keyboard_input = 'c';
                    break;
                }
            }

            break;

        case 'f':
            // start face detecting and after the app detects any face, start editing
            textlcd_write(keyboard_input, 0,0);
            printf("f pressed\n");
            printf("face detection start\n");

            while(keyboard_input != 'q'){
                // loop until only if user hasn't pressed 'q' or a face hasn't detected
                if(kbhit()){
                    keyboard_input = readch();
                }
                printf("keyboard input: %c \n", keyboard_input);

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
                    keyboard_input = 'q';
                }
            }

            break;

        case 'y' :
            //Gray Scaling
            textlcd_write(keyboard_input, 0,0);
            printf("y pressed\n");
            printf("Gray scaling start\n");
            /* cvIMG2GRAY(gray_scale, cis_rgb, img->width, img->height);	 */
            //this function is also used to make gray scaled video
            detect_and_draw_gray(temp_cv_image, fb_mapped, cis_rgb);

            break;

        default:
            printf("%c\n", keyboard_input);
            printf("undefined key pressed \n");
            break;

        }
    }


    if (GetTouch() != -1) {
      setFrame(x_detected, y_detected, brush_color, brush_size);
      LCD_print(fb_mapped);
    }
  }

  /* munmap(fb_mapped, mem_size); */
  /* close(fb_fd); */
  close_keyboard();
  close_camera();
  close_LCD();
  close_textlcd();
  close_dotmatrix();
  close_segment();
  close_facedetect();

  return 0;
}
