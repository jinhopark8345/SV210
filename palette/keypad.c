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

#define EVENT_BUF_NUM 64

void init_keypad(){
    int i, quit = 1;
    size_t read_bytes; // 몇 bytes read 했느냐
    struct input_event event_buf[EVENT_BUF_NUM]; // 몇개의 event 까지 한꺼번에 읽느냐
    keypad_fd = -1;

    // /dev/input/event3 파일을 읽기 전용 모드로 열어 // keypad_fd 변수에 파일 기술자를
    // 저장한다.
    if ((keypad_fd = open("/dev/input/event3", O_RDONLY)) < 0) {
        printf("application : keypad driver open fail!\n");
        exit(1);
    }

    printf("press the key button!\n");
    printf("press the key 26 to exit!\n");

    return;
}
void close_keypad(){
    close(keypad_fd);
    return;
}
