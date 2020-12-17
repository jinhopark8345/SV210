#ifndef KEYPAD_H
#define KEYPAD_H

#define KEYPAD11 1
#define KEYPAD12 2
#define KEYPAD13 3
#define KEYPAD14 4

#define KEYPAD21 5
#define KEYPAD22 6
#define KEYPAD23 7
#define KEYPAD24 8

#define KEYPAD31 9
#define KEYPAD32 10
#define KEYPAD33 11
#define KEYPAD34 12

#define KEYPAD41 13
#define KEYPAD42 14
#define KEYPAD43 15
#define KEYPAD44 16

#define SP_BRUSH_RGB 'w'
#define SP_BRUSH_RED 'r'
#define SP_BRUSH_GREEN 'g'
#define SP_BRUSH_BLUE 'b'

#define SP_BRUSH_ERASER 'e'
#define SP_BRUSH_SIZEUP '='
#define SP_BRUSH_SIZEDOWN '-'
#define SP_CAMERA 'c'

#define SP_FACEDETECTION 'f'
#define SP_GRAYSCALE 'y'
#define SP_LOAD_IMAGE 'L'

#define SP_STOP 'S'
#define SP_EXIT 'q'

#define SP_SAVE 's'

#define SP_UNDEFINED_INPUT 'x'
#define SP_EDITMODE 50

int keypad_fd;
int newInput_flag;

void init_keypad();
void close_keypad();
unsigned short read_keypad();
unsigned short translate_keypad();
char mapKeypadInput(u_int16_t keypad_input);

#endif
