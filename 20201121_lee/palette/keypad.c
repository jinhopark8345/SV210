#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "keypad.h"
#include "touchlcd.h"

#define EVENT_BUF_NUM 64

int Ok_flag = 0;

void init_keypad(){
    keypad_fd = -1;

    // /dev/input/event3 파일을 읽기 전용 모드로 열어 // keypad_fd 변수에 파일 기술자를
    // 저장한다.
    /* if ((keypad_fd = open("/dev/input/event3", O_RDONLY)) < 0) { */
    if ((keypad_fd = open("/dev/input/event3", O_RDONLY | O_NONBLOCK)) < 0) {
        printf("application : keypad driver open fail!\n");
        exit(1);
    }
    printf("initialize keypad!\n");

    return;
}

void close_keypad(){
    close(keypad_fd);
    return;
}

unsigned short read_keypad(){
    int i, quit = 1;
    size_t read_bytes; // 몇 bytes read 했느냐
    int rtv_input = -1;
    struct input_event event_buf[EVENT_BUF_NUM]; // 몇개의 event 까지 한꺼번에 읽느냐
    /* printf("no problem 1\n"); */
    // event 발생을 EVENT_BUF_NUM 만큼 읽어들인다.
    read_bytes = read(keypad_fd, event_buf, (sizeof(struct input_event) * EVENT_BUF_NUM));
    /* printf("no problem 2\n"); */

    if (read_bytes < sizeof(struct input_event)) {
        printf("keypad: read error!!");
        exit(1);
    }
    /* printf("no problem 3\n"); */
    /* printf("for loop range: %d\n", (read_bytes / sizeof(struct input_event))); */

    // 4 works,
    for (i = 0; i < 4; i++) {
    /* for (i = 0; i < 64; i++) { */
        // event type : KEY
        // event value : 0(pressed)
        if ((event_buf[i].type == EV_KEY) && (event_buf[i].value == 0)) {
            printf("\n Button key : %d, %d\n", event_buf[i].code, i);
            rtv_input = event_buf[i].code;
            rtv_input = translate_keypad(rtv_input);
            if (event_buf[i].code == 26) {
                printf("\napplication : Exit Program!! (key = %d)\n", event_buf[i].code);
                /* quit = 0; */
            }
        }
    }
    printf("\n");
    /* for (i = 0; i dlq< (read_bytes / sizeof(struct input_event)); i++) { */
    /*     // event type : KEY */
    /*     // event value : 0(pressed) */
    /*     if ((event_buf[i].type == EV_KEY) && (event_buf[i].value == 0)) { */
    /*         printf("\n Button key : %d\n", event_buf[i].code); */
    /*         rtv_input = event_buf[i].code; */

    /*         if (event_buf[i].code == 26) { */
    /*             printf("\napplication : Exit Program!! (key = %d)\n", event_buf[i].code); */
    /*             /\* quit = 0; *\/ */
    /*         } */
    /*     } */
    /* } */

    /* if(rtv_input == 0){ */

    /* } */

    //rtv_input = translate_keypad(rtv_input);

    return rtv_input;
}

unsigned short translate_keypad(int keypad_input){

    unsigned short user_input = 0;

    switch(keypad_input) {
    case -1: //
        user_input = 0;
        break;
    case 1:
        user_input = 'w';
        break;
    case 2:
        user_input = 'r';
        break;
    case 3:
        user_input = 'b';
        break;
    case 4:
        user_input = 'g';
        break;
    case 5:
        user_input = 'e';
        break;
    case 6:
        user_input = '+';
        break;
    case 7:
        user_input = '-';
        break;
    case 8:
        user_input = 's';
        break;
    case 9:
        user_input = 'c';
        break;

    case 10:
        user_input = 'f';
        break;
    case 11:
        user_input = 'y';
        break;
    default:
        printf("%c\n", user_input);
        printf("undefined keypad pressed \n");
        break;
    }



    return user_input;
}
