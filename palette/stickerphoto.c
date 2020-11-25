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
  init_touchlcd();
  init_keyboard();

  /* unsigned short white = (0b1111111111111111); */
  /* unsigned short red = (0b11111 << 11); */
  /* unsigned short blue = (0b111111 << 5); */
  /* unsigned short green = (0b11111); */
  /* unsigned short eraser = (0b11111); */
  /* unsigned short brush_color = 0; */
  unsigned short white = 1;
  unsigned short red = 2;
  unsigned short blue = 3;
  unsigned short green = 4;
  unsigned short eraser = 0;

  unsigned short keyboard_input=0;
  // default brush color
  unsigned short brush_color = green;
  char* file_name = "face_image.jpg";
  char* background_image = "background.bmp";
  unsigned char *fb_mapped;

  // prepare touchlcd
  init_palette(background_image);
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
                break;
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
  close_LCD();

  return 0;
}
