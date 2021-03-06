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

#include "camera.h"
#include "dipsw.h"
#include "dotmatrix.h"
#include "gpio.h"
#include "imagepr.h"
#include "keyboard.h"
#include "keypad.h"
#include "segment.h"
#include "stickerphoto.h"
#include "textlcd.h"
#include "touchlcd.h"

char user_input = SP_EDITMODE;

typedef struct set_color {
  unsigned int rgbcol; // only for segment
  unsigned short red;
  unsigned short blue;
  unsigned short green;
} set_color;
struct set_color col = {30, 30, 30, 30};

unsigned short get_color(unsigned short brush_color) {
  unsigned short tmp = 0;
  switch (brush_color) {
  case SP_BRUSH_RGB:
    brush_color = (col.red << 11 | col.green << 5 | col.blue);
    printf("brush color: rgb\n");
    break;
  case SP_BRUSH_RED:
    // brush_color = (0b11111 << 11);
    col.red += 6; // 0b11111/10  10 steps for 7 segment
    if (col.red >= 63) {
      col.red = 0;
    }
    col.rgbcol = (col.red / 6) * 10000 + col.green / 6 * 100 + col.blue / 6;
    brush_color = col.red << 10;
    printf("brush color: red, color level: %d\n", col.rgbcol);
    break;
  case SP_BRUSH_BLUE:
    // brush_color = (0b11111);
    col.blue += 6; // 0b111111/10  10 steps for 7 segment
    if (col.blue >= 63) {
      col.blue = 0;
    }
    col.rgbcol = (col.red / 6) * 10000 + col.green / 6 * 100 + col.blue / 6;
    brush_color = col.blue;
    printf("brush color: blue, color level: %d\n ", col.rgbcol);
    break;
  case SP_BRUSH_GREEN:
    // brush_color = (0b111111 << 5);
    col.green += 6; // 0b111111/10  10 steps for 7 segment
    if (col.green >= 63) {
      col.green = 0;
    }
    col.rgbcol = (col.red / 6) * 10000 + col.green / 6 * 100 + col.blue / 6;
    brush_color = col.green << 5;
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

void init_sp_camera(unsigned char *fb_mapped) {
  unsigned short user_input_tmp = 0;

  printf("c pressed, plain camera starts, save image with gpio button \n");
  printf("quit: press the same button(c) again \n");

  while (user_input != SP_STOP && user_input != SP_EXIT) {

    // after read_camera2rgb, cis_rgb values changed
    // change csframe value -> show camera image on touchlcd
    read_camera2rgb();
    change_palette_image(cis_rgb);

    // display changed csframe values
    LCD_print(fb_mapped);

    user_input_tmp = read_keypad();
    if (newInput_flag == 1) {
      printf("camera mode, new input detected, user_input: %d\n", user_input);
      newInput_flag = -1; // reset the flag
      user_input = user_input_tmp;
      textlcd_write(user_input, 0, 0);
    }

    if (gpio_button % 2 == 1) {
      save_touchlcd();
      gpio_button = 0;
    }
  }
}

void init_sp_grayscale(unsigned char *fb_mapped) {
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
void init_sp_facedetect(unsigned char *fb_mapped) {
  /* facedetect variables */
  int num_detected_face = 0;
  unsigned short user_input_tmp = 0;
  IplImage *temp_cv_image = cvCreateImage(cvSize(320, 240), IPL_DEPTH_8U, 3);

  // start face detecting and after the app detects any face, start editing
  printf("f pressed: face detection start \n");
  printf("press q to quit\n");

  while (user_input != SP_STOP && user_input != SP_EXIT) {

    // loop until only if user hasn't pressed 'S' or a face hasn't detected

    // after read_camera2rgb, cis_rgb values changed
    read_camera2rgb();

    // change csframe value -> show camera image on touchlcd
    change_palette_image(cis_rgb);

    // display changed csframe values
    LCD_print(fb_mapped);

    // detect face based on cis_rgb
    num_detected_face = detect_face(temp_cv_image, cis_rgb, fb_mapped);
    printf("%d detected!!\n", num_detected_face);

    if (num_detected_face > 0) {
      user_input = SP_STOP; // to show that camera has stoped in textlcd
      textlcd_write(user_input, 0, 0);
      /* dotmatrix_write(num_detected_face); */
      break;
    }

    user_input_tmp = read_keypad();
    if (newInput_flag == 1) {
      // reset the flag
      newInput_flag = -1;
      user_input = user_input_tmp;
      textlcd_write(user_input, 0, 0);
    }

    if (gpio_button % 2 == 1) {
      save_touchlcd();
      gpio_button = 0;
    }
  }

  cvReleaseImage(&temp_cv_image);
}

void save_touchlcd() {
  IplImage *temp_cv_image =
      cvCreateImage(cvSize(LCD_WIDTH, LCD_HEIGHT), IPL_DEPTH_8U, 3);
  u_int16_t i;
  printf("gpio button pressed: save image as '%s'\n", SAVE_FILE);
  if (DOTMATRIX_ON > 0) {
    for (i = 9; i > 0; i--) {
      dotmatrix_write(i);
      m_delay(10);
      dotmatrix_write(i);
      m_delay(10);
    }
  }

  textlcd_write(SP_SAVE, 0, 0);

  // save whole lcd
  RGB2cvIMG(temp_cv_image, csframe, LCD_WIDTH, LCD_HEIGHT);

  // save image as cv format
  cvSaveImage(SAVE_FILE, temp_cv_image);

  cvReleaseImage(&temp_cv_image);

  return;
}

void init_stickerphoto() {

  /* palette variables */
  unsigned short white = 1;
  unsigned short red = 2;
  unsigned short blue = 3;
  unsigned short green = 4;
  unsigned short eraser = 0;
  unsigned short user_input_tmp = 0;
  unsigned short brush_color = green;
  unsigned short segment_flag = 0;

  char *background_image = "background.bmp";

  IplImage *load_image = NULL;

  unsigned char *fb_mapped;
  unsigned short keypad_input = -1;
  unsigned short keypad_input_tmp = 0;
  int i;

  brush_color = get_color(SP_BRUSH_RGB);
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
  init_palette(background_image); // prepare touchlcd

  fb_mapped = lcdvar.fb_mapped;

  while (user_input != SP_EXIT) { // KEYPAD44

    /* need to update touchlcd to black screen even though touchlcd is off */
    if (TOUCHLCD_ON == 0) { // TOUCHLCD off
      LCD_print(fb_mapped);
      TOUCHLCD_ON = 1;
    }

    /* update touchlcd whenever there is a new touch to touchlcd */
    if (GetTouch() != -1) {
      setFrame(x_detected, y_detected, brush_color, brush_size);
      LCD_print(fb_mapped);
    }

    segment_write(&(col.rgbcol));
    dip_read();

    if (gpio_button % 2 == 1) {
      save_touchlcd();
      gpio_button = 0;
    }

    user_input_tmp = read_keypad();
    if (newInput_flag == 1) {
      // reset the flag
      newInput_flag = -1;
      user_input = user_input_tmp;

      switch (user_input) {
      case SP_EXIT:
        resetFrame(fb_mapped);
        printf("KEYPAD44 pressed: exit the program \n");
        break;
      case SP_BRUSH_RGB:
      case SP_BRUSH_RED:
      case SP_BRUSH_GREEN:
      case SP_BRUSH_BLUE:
      case SP_BRUSH_ERASER:
        brush_color = get_color(user_input);
        /* textlcd_write(user_input, 0, brush_color); */
        textlcd_write(user_input, brush_size, brush_color);
        break;
      case SP_BRUSH_SIZEUP:
        textlcd_write(user_input, brush_size, brush_color);
        printf("+ pressed \n");
        brush_size += BRUSH_STEP;
        printf("increase brush size, brush size: %d\n", brush_size);
        break;
      case SP_BRUSH_SIZEDOWN:
        textlcd_write(user_input, brush_size, brush_color);
        printf("- pressed \n");
        brush_size -= BRUSH_STEP;
        printf("reduce brush size, brush size: %d\n", brush_size);
        break;

      case SP_CAMERA:
        textlcd_write(user_input, brush_size, brush_color);
        init_sp_camera(fb_mapped);
        break;

      case SP_FACEDETECTION:
        textlcd_write(user_input, brush_size, brush_color);
        init_sp_facedetect(fb_mapped);
        break;

      case SP_GRAYSCALE:
        textlcd_write(user_input, brush_size, brush_color);
        init_sp_grayscale(fb_mapped);
        break;

      case SP_LOAD_IMAGE:
        textlcd_write(user_input, brush_size, brush_color);
        load_image = cvLoadImage(SAVE_FILE, CV_LOAD_IMAGE_COLOR);
        if (load_image == NULL) {
          printf("\nload image error!");
          return;
        }
        load_img2LCD(load_image);
        LCD_print(fb_mapped);

        break;

      case SP_STOP:
        textlcd_write(user_input, brush_size, brush_color);
        break;

      case SP_UNDEFINED_INPUT:
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
  }

  cvReleaseImage(&load_image);
  close_gpio();
  close_keypad();
  close_dipsw();
  close_keyboard();
  close_camera();
  close_LCD();
  close_textlcd();
  close_dotmatrix();
  close_segment();
  close_facedetect();
}

int main(void) {
  init_stickerphoto();
  return 0;
}
