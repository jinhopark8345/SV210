#ifndef GPIO_H
#define GPIO_H

#define SAVE_FILE "save_image.png"
unsigned short gpio_button;

int init_gpio();
void close_gpio();
void save_sp();

#endif
