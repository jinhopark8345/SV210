
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

#include "touchlcd.h"

// LCD 화면 프레임 저장 배열
unsigned short frame[384000];
/* unsigned short backframe[384000]; */
// 터치 받을 때 시간 넘어가면 통과할 때 사용
int poll_state;


struct pollfd poll_events;
// TextLCD 에 표시되는 값들에 관한 정보 저장 구조체
struct strcommand_varible strcommand;
struct strcommand_varible {
    char rows;
    char nfonts;
    char display_enable;
    char cursor_enable;
    char nblink;
    char set_screen;
    char set_rightshit;
    char increase;
    char nshift;
    char pos;
    char command;
    char strlength;
    char buf[16];
};
struct strcommand_varible strcommand;
// file descriptors
int key_fd = -1;
int textlcd_fd = -1;
int dot_fd = -1;
int seg_fd = -1;
int led_fd = -1;
int event_fd = -1;
int buz_fd = -1;
struct input_event event_buf[EVENT_BUF_NUM];

// device driver들 로드
void init_touchlcd() {
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
}
// ms delay 함수
void m_delay(int num) {
    volatile int i, j;
    for (i = 0; i < num; i++)
        for (j = 0; j < 16384; j++)
            ;
}
int GetTouch(void) {
    int i;
    size_t read_bytests;
    struct input_event event_bufts[EVENT_BUF_NUM];
    int x, y;
    int index = 0;
    if ((event_fd = open("/dev/input/event2", O_RDONLY)) < 0) {
        printf("open error");
        exit(1);
    }
    poll_events.fd = event_fd;
    poll_events.events = POLLIN | POLLERR; // 수신된 자료가 있는지, 에러가 있는지
    poll_events.revents = 0;
    poll_state = poll( // poll()을 호출하여 event 발생 여부 확인
                      (struct pollfd *)&poll_events, // event 등록 변수
                      1,                             // 체크할 pollfd 개수
                      10                             // time out 시간
                       );
    if (poll_events.revents == POLLIN) {
        read_bytests = read(event_fd, event_bufts,
                            ((sizeof(struct input_event)) * EVENT_BUF_NUM));

        for (i = 0; i < (read_bytests / sizeof(struct input_event)); i++) {
            /* printf("event bufts %d\n", event_bufts[i].type); */
            switch (event_bufts[i].type) {
            case EV_ABS:
                switch (event_bufts[i].code) {
                case ABS_X:
                    y = event_bufts[i].value;
                    break;
                case ABS_Y:
                    x = event_bufts[i].value;
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }

        x_detected = (x / 20) - 1;
        if (x_detected >= LCD_WIDTH) {
            x_detected = LCD_WIDTH - 1;
        }

        if (x_detected <= 0) {
            x_detected = 0;
        }

        y_detected = (LCD_HEIGHT - y / 34);
        if (y_detected <= 0) {
            y_detected = 0;
        }

        // remove when pos spark
        /* if (x_detected < LCD_SPARK || y_detected < LCD_SPARK){ */
        /*     x_detected = x_detected_prev; */
        /*     y_detected = y_detected_prev; */
        /* } */
        if (x_detected < LCD_SPARK || x_detected > LCD_WIDTH - LCD_SPARK) {
            x_detected = x_detected_prev;
        }

        if (y_detected < LCD_SPARK || y_detected > LCD_HEIGHT - LCD_SPARK) {
            y_detected = y_detected_prev;
        }

        /* if (x_detected > LCD_WIDTH - LCD_SPARK || y_detected < LCD_HEIGHT -
         * LCD_SPARK){ */
        /*     x_detected = x_detected_prev; */
        /*     y_detected = y_detected_prev; */
        /* } */

        /* printf("x, y: %d,%d\n", x_detected, y_detected); */

        close(event_fd);
    } else {
        close(event_fd);
        return -1;
    }
    return 0;
}
void LCD_print(unsigned char *fb_mapped){
    int coor_y, coor_x;
    int ystart = 0;
    unsigned short *ptr;
    /*
    // control the area that user can edit
    // PALETTE HEIGHT and WIDTH define the size of the palette
    // PALETTE_START_Y and PALETTE_START_X define the start pos of the palette in the touchlcd
    */
    for (coor_y = 0; coor_y < PALETTE_HEIGHT; coor_y++) {
        ystart = (LCD_WIDTH * PALETTE_START_Y + PALETTE_START_X) +(LCD_WIDTH * coor_y);
        ptr = (unsigned short *)fb_mapped + ystart;
        for (coor_x = 0; coor_x < PALETTE_WIDTH; coor_x++)
            *ptr++ = csframe[coor_x + ystart];
    }

    /* printf("start point value %d\n", csframe[PALETTE_START_X+1 +(PALETTE_START_Y+1) * LCD_WIDTH]); */

}


struct lcd_variable init_palette(char* background, char* face_file){
    brush_size = 10;
    int screen_width;
    int screen_height;
    int bits_per_pixel;
    int line_length;
    /* int fb_fd; */
    struct fb_var_screeninfo fbvar;
    struct fb_fix_screeninfo fbfix;
    /* unsigned char *fb_mapped; */
    /* int mem_size; */
    unsigned short *ptr;
    int coor_y, coor_x;
    int i;
    FILE *bmpfd;
    char *lpImg, *tempImg;

    FILE *bmpfd_face;
    char *lpImg_face, *tempImg_face;

    char r, g, b;
    int j = 0;
    int cols = LCD_WIDTH, rows = LCD_HEIGHT;
    int ystart = 0;

    if (access(FBDEV_FILE, F_OK)) {
        printf("%s: access error\n", FBDEV_FILE);
        exit(1);
    }
    if ((lcdvar.fb_fd = open(FBDEV_FILE, O_RDWR)) < 0) {
        printf("%s: open error\n", FBDEV_FILE);
        exit(1);
    }
    if (ioctl(lcdvar.fb_fd, FBIOGET_VSCREENINFO, &fbvar)) {
        printf("%s: ioctl error - FBIOGET_VSCREENINFO \n", FBDEV_FILE);
        exit(1);
    }
    if (ioctl(lcdvar.fb_fd, FBIOGET_FSCREENINFO, &fbfix)) {
        printf("%s: ioctl error - FBIOGET_FSCREENINFO \n", FBDEV_FILE);
        exit(1);
    }
    screen_width = fbvar.xres; // 스크린의 픽셀 폭
    screen_height = fbvar.yres; // 스크린의 픽셀 높이
    bits_per_pixel = fbvar.bits_per_pixel; // 픽셀 당 비트 개수
    line_length = fbfix.line_length;       // 한개 라인 당 바이트 개수
    lcdvar.mem_size = screen_width * screen_height * 2;
    lcdvar.fb_mapped = (unsigned char *)mmap(0, lcdvar.mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, lcdvar.fb_fd, 0);

    bmpfd = fopen(background, "rb"); //파일을 읽기 모드로 엶
    if (bmpfd == NULL) {
        printf("background image open error\n");
        exit(1);
    }
    fseek(bmpfd, 54, SEEK_SET);
    lpImg = (char *)malloc(1152000);
    tempImg = lpImg;
    fread(lpImg, sizeof(char), 1152000, bmpfd);

    for (i = 0; i < 384000; i++) {
        b = *lpImg++;
        g = *lpImg++;
        r = *lpImg++;
        frame[j] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
        j++;
    }



    // setting the default frame: make the edit area white
    for (coor_y = 0; coor_y < PALETTE_HEIGHT; coor_y++) {
        ystart = (screen_width * PALETTE_START_Y + PALETTE_START_X) +(screen_width * coor_y);
        ptr = (unsigned short *)lcdvar.fb_mapped + ystart;
        for (coor_x = 0; coor_x < PALETTE_WIDTH; coor_x++){
            frame[coor_x + ystart] = DEFAULT_PALETTE_COLOR;
        }
    }


    // copy value from frame to csframe
    for (i = 0; i < 384000; i++) {
        csframe[i] = frame[i];
    }
    j = 0;

    // display rgb values in csframe
    for (coor_y = 0; coor_y < rows; coor_y++) {
        ptr = (unsigned short *)lcdvar.fb_mapped + (screen_width * coor_y);
        for (coor_x = 0; coor_x < cols; coor_x++)
            *ptr++ = csframe[coor_x + coor_y * cols];
    }

}

/*
  draw circle on the image
  position: (x,y)
  color: color of the brush
  radius: size of the brush
*/
void setFrame(int x, int y, unsigned short brush_color, int radius) {
    int j = 0;
    int i = 0;
    int radiusPow = 0;
    int tempPosPow = 0;
    int tempPos = 0;
    int tempX = 0, tempY = 0;
    /* unsigned short brush_color; */
    radiusPow = radius * radius;

    // if it's not an eraser
    // brush has circle shape
    for (j = -radius; j < radius; j++) {
        for (i = -radius; i < radius; i++) {
            tempX = i + x;
            tempY = j + y;

            // check if current position is inside of the LCD boundary
            if (tempX < 0)
                tempX = 0;
            if (tempX >= LCD_WIDTH)
                tempX = LCD_WIDTH - 1;
            if (tempY < 0)
                tempY = 0;
            if (tempY >= LCD_HEIGHT)
                tempY = LCD_HEIGHT - 1;

            // check if it's in the circle
            tempPos = tempX + tempY * LCD_WIDTH;
            tempPosPow = (i * i + j * j);
            if (tempPos >= 0 && tempPosPow <= radiusPow) {
                // printf("temppospow: %d\n",tempPosPow);
                /* printf("i,j : %d, %d\n",i,j); */
                /* printf("curpos : %d\n",curPos); */
                /* printf("tempPos : %d\n",tempPos ); */

                if (brush_color != PALETTE_ERASER) { // other than eraser
                    csframe[tempPos] = brush_color;
                } else {
                    // eraser
                    // untested
                    csframe[tempPos] = frame[tempPos];
                }

                // save previous x,y pos
                x_detected_prev = x_detected;
                y_detected_prev = y_detected;
            }
        }
    }
}

void change_palette_image(unsigned short *rgb){
	int coor_x, coor_y;
	/* int screen_width; */
	unsigned short *ptr;
    int ystart;
    for (coor_y = 0; coor_y < PALETTE_IMAGE_HEIGHT; coor_y++) {
        ystart = (LCD_WIDTH * PALETTE_IMAGE_START_Y + PALETTE_IMAGE_START_X) +(LCD_WIDTH * coor_y);
        ptr = (unsigned short *)lcdvar.fb_mapped + ystart;
        for (coor_x = 0; coor_x < PALETTE_IMAGE_WIDTH; coor_x++){
            /* frame[coor_x + ystart] = DEFAULT_PALETTE_COLOR; */
            frame[coor_x + ystart] = rgb[coor_x + coor_y * PALETTE_IMAGE_WIDTH];
            csframe[coor_x + ystart] = rgb[coor_x + coor_y * PALETTE_IMAGE_WIDTH];
        }
    }
}


void close_LCD(){
    printf("release current lcdvar struct\n");
    munmap(lcdvar.fb_mapped,lcdvar.mem_size);
    close(lcdvar.fb_fd);
}
