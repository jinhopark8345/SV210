#include <fcntl.h>
#include <fcntl.h> // O_RDWR
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/ioctl.h> // ioctl
#include <sys/ioctl.h>
#include <sys/mman.h> // mmap PROT_
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <unistd.h> // open/close
#include <unistd.h>
// touch 관련 변수 정의
#define EVENT_BUF_NUM 2

#define LCD_WIDTH 800
#define LCD_HEIGHT 480
#define LCD_SPARK 3

int x_detected, y_detected; // touch 받아서 detecting한 좌표 저장하는 변수
int x_detected_prev, y_detected_prev;
int brush_size = 10;
#define FBDEV_FILE "/dev/fb0"

// LCD 관련 정의
#define LCD_FINIT 0
#define LCD_SINIT 1
#define LCD_PRINT 2
// LCD 화면 프레임 저장 배열
unsigned short frame[384000];
unsigned short csframe[384000];
/* unsigned short backframe[384000]; */
// 커서의 현재 좌표 저장용 배열
struct timeval val;
struct tm *ptm;
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

/*
  draw circle on the image
  position: (x,y)
  color: color of the brush
  radius: size of the brush

*/
void setFrame(int x, int y, int color, int radius) {
  int j = 0;
  int i = 0;
  int radiusPow = 0;
  int tempPosPow = 0;
  int tempPos = 0;
  int tempX = 0, tempY = 0;
  unsigned short brush_color;
  radiusPow = radius * radius;

  switch (color) {
  case 0: // eraser
    break;
  case 1: // white
    brush_color = (0b1111111111111111);
    break;
  case 2: // red
    brush_color = (0b11111 << 11);
    break;
  case 3: // blue
    brush_color = (0b111111 << 5);
    break;
  case 4: // green
    brush_color = (0b11111);
    break;
  default:
    break;
  }

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

        if (color != 0) { // other than eraser
          csframe[tempPos] = brush_color;
        } else { // eraser
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
void LCDinit(int inp) {
  int screen_width;
  int screen_height;
  int bits_per_pixel;
  int line_length;
  int fb_fd;
  struct fb_var_screeninfo fbvar;
  struct fb_fix_screeninfo fbfix;
  unsigned char *fb_mapped;
  int mem_size;
  unsigned short *ptr;
  int coor_y;
  int coor_x;
  int i;
  FILE *bmpfd;
  char *lpImg, *tempImg;
  char r, g, b;
  int j = 0;
  int cols = LCD_WIDTH, rows = LCD_HEIGHT;
  int disable_height = brush_size * 2;
  if (access(FBDEV_FILE, F_OK)) {
    printf("%s: access error\n", FBDEV_FILE);
    exit(1);
  }
  if ((fb_fd = open(FBDEV_FILE, O_RDWR)) < 0) {
    printf("%s: open error\n", FBDEV_FILE);
    exit(1);
  }
  if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &fbvar)) {
    printf("%s: ioctl error - FBIOGET_VSCREENINFO \n", FBDEV_FILE);
    exit(1);
  }
  if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &fbfix)) {
    printf("%s: ioctl error - FBIOGET_FSCREENINFO \n", FBDEV_FILE);
    exit(1);
  }
  screen_width = fbvar.xres; // 스크린의 픽셀 폭
  /* printf("screen width: %d\n", screen_width); */
  screen_height = fbvar.yres; // 스크린의 픽셀 높이
  /* printf("screen height: %d\n", screen_height); */
  bits_per_pixel = fbvar.bits_per_pixel; // 픽셀 당 비트 개수
  line_length = fbfix.line_length;       // 한개 라인 당 바이트 개수
  mem_size = screen_width * screen_height * 2;
  fb_mapped = (unsigned char *)mmap(0, mem_size, PROT_READ | PROT_WRITE,
                                    MAP_SHARED, fb_fd, 0);
  if (inp == 0) {
    bmpfd = fopen("size2.bmp", "rb"); //파일을 읽기 모드로 엶
    if (bmpfd == NULL) {
      printf("파일 소환 실패\n");
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
    for (i = 0; i < 384000; i++) {
      csframe[i] = frame[i];
    }
    j = 0;
    for (coor_y = 0; coor_y < rows; coor_y++) {
      ptr = (unsigned short *)fb_mapped + (screen_width * coor_y);
      for (coor_x = 0; coor_x < cols; coor_x++)
        *ptr++ = csframe[coor_x + coor_y * cols];
    }
    fclose(bmpfd);
  }
  if (inp == 2) {
    for (coor_y = 0; coor_y < rows - disable_height; coor_y++) {
      ptr = (unsigned short *)fb_mapped + (screen_width * coor_y);
      for (coor_x = 0; coor_x < cols; coor_x++)

        *ptr++ = csframe[coor_x + coor_y * cols];
    }
  }
  munmap(fb_mapped, mem_size);
  close(fb_fd);
}
// device driver들 로드
void init_devices() {
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
int main(void) {
  init_devices();
  LCDinit(LCD_FINIT);

  /* unsigned short white = (0b1111111111111111); */
  /* unsigned short red = (0b11111 << 11); */
  /* unsigned short blue = (0b111111 << 5); */
  /* unsigned short green = (0b11111); */
  /* unsigned short eraser = (0b11111); */
  /* unsigned short brush_color = 0; */
  unsigned short white = 1;
  unsigned short red = 2;
  unsigned short blue = 3;
  unsigned short eraser = 0;

  // default brush color
  unsigned short brush_color = red;

  while (1) {
    brush_color = white;
    brush_size = 15;
    if (GetTouch() != -1) {
      setFrame(x_detected, y_detected, brush_color, brush_size);
      LCDinit(LCD_PRINT);
    }
  }
  return 0;
}