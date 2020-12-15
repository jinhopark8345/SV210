
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
int time_interval = 500000;
struct timeval lasttv;

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

int is_newInput(struct timeval newtv){
    int secDiff;
    int usecDiff;
    int msecDiff;

    secDiff = newtv.tv_sec - lasttv.tv_sec;
    if (newtv.tv_usec >= lasttv.tv_usec){
        usecDiff = newtv.tv_usec - lasttv.tv_usec;
    }{
        usecDiff = lasttv.tv_usec - newtv.tv_usec;
    }

    msecDiff = usecDiff / 1000 + secDiff * 1000;

    printf("msecDiff : %d\n" , msecDiff);
    if (msecDiff < 700){
        printf("msecDiff : %d\n" , msecDiff);

        // update last time interval
        lasttv = newtv;

        return 1;
    }

    /* if(secDiff <= 1 || secDiff > -1 ){ */
    /*     return true; */
    /* } */



    return -1;
}

unsigned short read_keypad(){
    int i, quit = 1;
    size_t read_bytes; // 몇 bytes read 했느냐
    int rtv_input = -1;
    struct input_event event_buf[EVENT_BUF_NUM]; // 몇개의 event 까지 한꺼번에 읽느냐
    struct timeval tv;
    struct timeval newtv;
    int newInput = -1;
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

    /* for (i = 0; i< 4;i++){ */
    /*     // event_buf[i].value == 1 : keypress */
    /*     if ((event_buf[i].type == EV_KEY) && (event_buf[i].value == 0)) { */

    /*         tv = event_buf[i].time; */
    /*         gettimeofday(&tv, NULL); */
    /*         printf("event_buf[%d].type == %d, event_buf[%d].value == %d, event_buf[%d].code == %d, time: %d.%d \n", i, event_buf[i].type, i, event_buf[i].value, i, event_buf[i].code, tv.tv_sec, tv.tv_usec); */
    /*         /\* printf("event_buf[%d].time == %d\n", i, tv.tv_usec ); *\/ */
    /*     } */
    /* } */

    // 4 works,
    Ok_flag = 0;//for iteration in stickerphoto.c
    /* for (i = 0; i < EVENT_BUF_NUM; i++) { */
    for (i = 0; i < 4; i++) {
        if ((event_buf[i].type == EV_KEY) && (event_buf[i].value == 0)) {
            gettimeofday(&newtv, NULL);
            if (is_newInput(newtv) > 0){
                printf("\n Button key : %d, %d\n", event_buf[i].code, i);
                rtv_input = event_buf[i].code;
                rtv_input = translate_keypad(rtv_input);
                event_buf[i].value = 1;
                Ok_flag = 1;
            }

        }
    }
    printf("\n");


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
    case 13:
        user_input = 'l';
        break;
    case 16://for ESC
        user_input =  16;
    default:
        printf("%c\n", user_input);
        printf("undefined keypad pressed \n");
        break;
    }



    return user_input;
}
