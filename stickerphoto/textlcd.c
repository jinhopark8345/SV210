#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "dipsw.h"
#include "keypad.h"
#include "textlcd.h"

char pad_char = 32; // space
int text_dev;
struct strcommand_varible strcommand;

void init_textlcd() {
  strcommand.rows = 0;
  strcommand.nfonts = 0;
  strcommand.display_enable = 1;
  strcommand.cursor_enable = 0;
  strcommand.nblink = 0;
  strcommand.set_screen = 0;
  strcommand.set_rightshit = 1;
  strcommand.increase = 1;
  strcommand.nshift = 0;
  strcommand.pos = 10;
  strcommand.command = 1;
  strcommand.strlength = 16;
  text_dev = open("/dev/textlcd", O_WRONLY | O_NDELAY);
  if (text_dev == -1) {
    printf("textlcd device driver not open!!");
  }
}

void print_str(char *str) {
  int i;
  for (i = 0; i < strlen(str); i++) {
    printf("%c", str[i]);
  }

  printf("\n");
}

void _textlcd_write(char *str1, char *str2) {
  strcommand.pos = 0;
  ioctl(text_dev, TEXTLCD_CLEAR, &strcommand, 32);
  write(text_dev, str1, 16);

  strcommand.pos = 40;
  ioctl(text_dev, TEXTLCD_DD_ADDRESS, &strcommand, 32);
  write(text_dev, str2, 16);

  /* print_str(str1); */
  /* print_str(str2); */
  /* printf("str1: %s\n", str1); */
  /* printf("str2: %s\n", str2); */
}

void resetStr(char *str) {
  // reset dstStr
  int i;
  for (i = 0; i < 16; i++) {
    str[i] = pad_char;
  }
  str[16] = '\0';

  return;
}

void textlcd_write(char ch, unsigned short brush_size,
                   unsigned short brush_color) {
  int i = 0, j = 0;
  unsigned short tmp_red, tmp_green, tmp_blue;
  char *flStr = malloc(16 * sizeof(char)); // textlcd first line string
  char *slStr = malloc(16 * sizeof(char)); // textlcd second line string
  char *tempStr = malloc(16 * sizeof(char));
  char *emptyStr = "                ";
  /* char tempStr[16]; */

  if (!(TEXTLCD_ON > 0)) { // TEXTLCD off
    // clear textlcd
    ioctl(text_dev, TEXTLCD_CLEAR, &strcommand, 32);
    write(text_dev, "                ", 16);
    strcommand.pos = 40;
    ioctl(text_dev, TEXTLCD_DD_ADDRESS, &strcommand, 32);
    strcommand.pos = 0;
    ioctl(text_dev, TEXTLCD_DD_ADDRESS, &strcommand, 32);

  } else { // TEXTLCD on
    switch (ch) {
    case SP_BRUSH_RGB:
      resetStr(flStr);
      resetStr(slStr);

      pad_string("SELECTED: RGB", flStr);
      pad_string(emptyStr, slStr);
      _textlcd_write(flStr, slStr);

      break;
    case SP_BRUSH_RED:

      resetStr(flStr);
      resetStr(slStr);

      pad_string("SELECTED: RED", flStr);

      tmp_red = (brush_color >> 10) / 6;
      /* printf("tmp_red: %d\n", tmp_red); */
      sprintf(tempStr, "Color level: %d", tmp_red);
      pad_string(tempStr, slStr);

      _textlcd_write(flStr, slStr);
      break;
    case SP_BRUSH_BLUE:

      resetStr(flStr);
      resetStr(slStr);

      pad_string("SELECTED: BLUE", flStr);

      tmp_blue = brush_color / 6;
      /* printf("tmp_blue: %d\n", tmp_blue); */
      sprintf(tempStr, "Color level: %d", tmp_blue);
      pad_string(tempStr, slStr);

      _textlcd_write(flStr, slStr);

      break;
    case SP_BRUSH_GREEN:

      resetStr(flStr);
      resetStr(slStr);

      pad_string("SELECTED: GREEN", flStr);

      tmp_green = (brush_color >> 5) / 6;
      sprintf(tempStr, "Color level: %d", tmp_green);
      pad_string(tempStr, slStr);

      _textlcd_write(flStr, slStr);

      break;
    case SP_BRUSH_ERASER:
      resetStr(flStr);
      resetStr(slStr);

      pad_string("SELECTED: ERASER", flStr);
      pad_string(emptyStr, slStr);

      _textlcd_write(flStr, slStr);

      break;
    case SP_BRUSH_SIZEUP:
      resetStr(flStr);
      resetStr(slStr);

      pad_string("BRUSH SIZE UP", flStr);
      sprintf(tempStr, "BRUSH SIZE: %d", brush_size);
      pad_string(tempStr, slStr);

      _textlcd_write(flStr, slStr);

      break;
    case SP_BRUSH_SIZEDOWN:
      resetStr(flStr);
      resetStr(slStr);

      pad_string("BRUSH SIZE DOWN", flStr);
      sprintf(tempStr, "BRUSH SIZE: %d", brush_size);
      pad_string(tempStr, slStr);

      _textlcd_write(flStr, slStr);

      break;

    case SP_CAMERA:
      resetStr(flStr);
      resetStr(slStr);

      pad_string("START", flStr);
      pad_string("CAMERA", slStr);

      _textlcd_write(flStr, slStr);
      break;
    case SP_FACEDETECTION:
      resetStr(flStr);
      resetStr(slStr);

      pad_string("START", flStr);
      pad_string("FACE DETECTION", slStr);

      _textlcd_write(flStr, slStr);
      break;

    case SP_GRAYSCALE:
      resetStr(flStr);
      resetStr(slStr);

      pad_string("APPLY", flStr);
      pad_string("GRAYSCALE", slStr);

      _textlcd_write(flStr, slStr);

      break;
    case SP_LOAD_IMAGE:
      resetStr(flStr);
      resetStr(slStr);

      pad_string("LOAD", flStr);
      pad_string("SAVED IMAGE", slStr);

      _textlcd_write(flStr, slStr);

      break;
    case SP_STOP:
      resetStr(flStr);
      resetStr(slStr);

      pad_string("STOP", flStr);
      pad_string("CAMERA", slStr);

      _textlcd_write(flStr, slStr);

      break;

    case SP_EXIT:
      resetStr(flStr);
      resetStr(slStr);

      pad_string("SELECTED: EXIT", flStr);
      pad_string("EXIT PROGRAM", slStr);

      _textlcd_write(flStr, slStr);

      break;

    case SP_SAVE:
      resetStr(flStr);
      resetStr(slStr);

      pad_string("SAVE ", flStr);
      pad_string("TEXTLCD IMAGE", slStr);

      _textlcd_write(flStr, slStr);

      break;
    case SP_UNDEFINED_INPUT:
    case SP_EDITMODE:

      resetStr(flStr);
      resetStr(slStr);

      pad_string("EDIT MODE", flStr);
      pad_string("INPUT", slStr);

      _textlcd_write(flStr, slStr);
      /* ioctl(text_dev,TEXTLCD_CLEAR,&strcommand,32); */
      /* write(text_dev,"SET DIPSW 255   ",16); */
      /* strcommand.pos = 40; */
      /* ioctl(text_dev,TEXTLCD_DD_ADDRESS,&strcommand,32); */
      break;
    default:
      break;
    }
  }

  free(flStr);
  free(slStr);
  free(tempStr);
}

void close_textlcd() { close(text_dev); }

void pad_string(char *srcStr, char *dstStr) {
  int len_src = strlen(srcStr);
  int i, j = 0;
  int padding = 0;

  padding = (16 - len_src) / 2;
  if (padding < 0)
    padding = 0;

  /* for (i= 0;i<16;i++){ */
  /*     if (i>= padding || i < len_src + padding){ */
  /*         dstStr[i] = srcStr[j]; */
  /*         ++j; */
  /*     }else{ */
  /*         dstStr[i] = pad_char; */
  /*     } */

  /* } */

  for (i = padding; i < len_src + padding; i++) {
    dstStr[i] = srcStr[j];
    ++j;
  }
  dstStr[16] = '\0';
}
