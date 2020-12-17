
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
#include "dipsw.h"

#define KEYPAD_EVENT_BUF_NUM 64
#define TIME_INTERVAL_MSEC 1000

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
    }else {
        usecDiff = lasttv.tv_usec - newtv.tv_usec;
    }

    msecDiff = usecDiff / 1000 + secDiff * 1000;
    if (msecDiff > TIME_INTERVAL_MSEC){
        /* printf("\n"); */
        /* printf("lasttv sec : %d\n", lasttv.tv_sec); */
        /* printf("newtv sec : %d\n", newtv.tv_sec); */
        /* printf("sec diff: %d, msec diff: %d, usec diff: %d\n", secDiff, msecDiff, usecDiff); */
        /* printf("msecDiff : %d\n" , msecDiff); */
        /* printf("usec diff: %d\n", usecDiff); */
        return 1;
    }


    return -1;
}

unsigned short read_keypad(){
    int i, quit = 1;
    size_t read_bytes; // 몇 bytes read 했느냐
    char rtv_input = -1;
    struct input_event event_buf[KEYPAD_EVENT_BUF_NUM]; // 몇개의 event 까지 한꺼번에 읽느냐
    struct timeval tv;
    struct timeval newtv;
    /* printf("no problem 1\n"); */
    // event 발생을 KEYPAD_EVENT_BUF_NUM 만큼 읽어들인다.
    read_bytes = read(keypad_fd, event_buf, (sizeof(struct input_event) * KEYPAD_EVENT_BUF_NUM));
    /* printf("no problem 2\n"); */

    if (read_bytes < sizeof(struct input_event)) {
        printf("keypad: read error!!");
        exit(1);
    }


    gettimeofday(&newtv, NULL);
    newInput_flag = is_newInput(newtv);

    if (newInput_flag == 1){

        // clear the buffer, so it run on main func
        if(!(KEYPAD_ON > 0)){
            newInput_flag = 0;
        }

        /* printf("time : %d.%d\n", newtv.tv_sec, newtv.tv_usec); */
        for (i = 0; i < 4; i++) {
            if ((event_buf[i].type == EV_KEY) && (event_buf[i].value == 0)) {
                /* rtv_input = event_buf[i].code; */
                /* rtv_input = mapKeypadInput(rtv_input); */
                rtv_input = mapKeypadInput(event_buf[i].code);
                event_buf[i].value = 1;

                printf("user_input: %c\n", rtv_input);

            }
        }

        // update last time
        lasttv = newtv;
    }


    return rtv_input;
}
char mapKeypadInput(u_int16_t keypad_input){

    switch(keypad_input){

    case KEYPAD11: return SP_BRUSH_RGB;
    case KEYPAD12: return SP_BRUSH_RED;
    case KEYPAD13: return SP_BRUSH_GREEN;
    case KEYPAD14: return SP_BRUSH_BLUE;

    case KEYPAD21: return SP_BRUSH_ERASER;
    case KEYPAD22: return SP_BRUSH_SIZEUP;
    case KEYPAD23: return SP_BRUSH_SIZEDOWN;

    case KEYPAD31: return SP_FACEDETECTION;
    case KEYPAD32: return SP_GRAYSCALE;
    case KEYPAD33: return SP_CAMERA;
    /* case KEYPAD33: return; */
    /* case KEYPAD34: return; */

    /* case KEYPAD41: */
    case KEYPAD42: return SP_LOAD_IMAGE;
    case KEYPAD43: return SP_STOP;
    case KEYPAD44: return SP_EXIT;

    default: return SP_UNDEFINED_INPUT;

    }
}
