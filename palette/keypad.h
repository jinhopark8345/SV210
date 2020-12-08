#ifndef KEYPAD_H
#define KEYPAD_H

int keypad_fd;

void init_keypad();
void close_keypad();
unsigned short read_keypad();
unsigned short translate_keypad();

#endif
