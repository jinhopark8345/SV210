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



unsigned short get_color(unsigned short brush_color){
    printf("%c pressed\n", brush_color);
    switch (brush_color) {
    case 'w': // white
        brush_color = (0b1111111111111111);
        printf("brush color: white\n");
        break;
    case 'r': // red
        brush_color = (0b11111 << 11);
        printf("brush color: red\n");
        break;
    case 'b': // blue
        brush_color = (0b11111);
        printf("brush color: blue\n");
        break;
    case 'g': // green
        brush_color = (0b111111 << 5);
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
  init_touchlcd(); //
  init_keyboard();
  init_camera(); // camer device file : camera_fd
  init_facedetect();

  /* facedetect variables */
  int ret = 0;

  /* palette variables */
  unsigned short white = 1;
  unsigned short red = 2;
  unsigned short blue = 3;
  unsigned short green = 4;
  unsigned short eraser = 0;
  unsigned short keyboard_input=0;
  unsigned short brush_color = green;

  char* face_file = "face_image.jpg";
  char* background_image = "background.bmp";
  unsigned char *fb_mapped;

  // prepare touchlcd
  init_palette(background_image, face_file);
  fb_mapped = lcdvar.fb_mapped;

  while (keyboard_input != 'q') {
    if(kbhit()){
        keyboard_input = readch();

        switch(keyboard_input) {
            case 'w':
                brush_color = get_color(keyboard_input);
                break;
            case 'r':
                brush_color = get_color(keyboard_input);
                break;
            case 'b':
                brush_color = get_color(keyboard_input);
                break;
            case 'g':
                brush_color = get_color(keyboard_input);
                break;
            case 'e':
                brush_color = get_color(keyboard_input);
                break;
            case '+':
                printf("+ pressed \n");
                brush_size += BRUSH_STEP;
                printf("increase brush size, brush size: %d\n",brush_size);
                break;
            case '-':
                printf("- pressed \n");
                brush_size -= BRUSH_STEP;
                printf("reduce brush size, brush size: %d\n",brush_size);

            case 'c':
                // case c havne't tested
                // just plain camera, no image processing

                printf("c pressed \n");
                // after read_camera2rgb, cis_rgb values changed
                read_camera2rgb();
                // change csframe value -> show camera image on touchlcd
                change_palette_image(cis_rgb);
            case 'f':
                printf("f pressed\n");
                printf("face detection start\n");

                // while loop, until the detect_face finishes, program can't get out here

                // without while loop, this should be finished immediately without detecting faces
                // withou while loop, hopefully implementation works
                ret = detect_face(cis_rgb, fb_mapped);
                printf("ret value: %d\n", ret);

                while(ret == 0){
                    // detect face until app finds at least one face
                    ret = detect_face(cis_rgb, fb_mapped);
                }

                // phase#1 should be real time camera in touchlcd
                // when it detects any face, phase#2 begins, show the
                // detected image on the palette and make the image editable



            default:
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

  close_facedetect();

  return 0;
}
