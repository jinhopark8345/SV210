#ifndef GPIO_H
#define GPIO_H

unsigned short gpio_button;

int init_gpio();
void close_gpio();
// int kbhit();
void read_gpio();

#endif
