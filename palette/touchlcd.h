#ifndef TOUCHLCD_H
#define TOUCHLCD_H

// int brush_size = 10;
int brush_size;
int x_detected, y_detected; // touch 받아서 detecting한 좌표 저장하는 변수
int x_detected_prev, y_detected_prev;

/* #define DEFAULT_PALETTE_COLOR 0b0000000000000000 */ //black
#define DEFAULT_PALETTE_COLOR 0b1111111111111111 // white

#define PALETTE_ERASER 0b1011111111111111

#define BRUSH_STEP 2

// touch 관련 변수 정의
#define EVENT_BUF_NUM 2

#define LCD_WIDTH 800
#define LCD_HEIGHT 480
#define PALETTE_WIDTH 400
#define PALETTE_HEIGHT 320
#define PALETTE_START_X 395
#define PALETTE_START_Y 80
#define LCD_SPARK 3

#define FBDEV_FILE "/dev/fb0"

// LCD 관련 정의
#define LCD_FINIT 0
#define LCD_PRINT 2


/* struct lcd_variable lcdvar; */
struct lcd_variable{
  unsigned char *fb_mapped;
  int mem_size;
  int fb_fd;
};
struct lcd_variable lcdvar;


void init_touchlcd();

void m_delay(int num);

int GetTouch(void);

void LCD_print(unsigned char *fb_mapped);

// unsigned char LCDinit(char* background);
// struct lcd_variable LCDinit(char* background);
struct lcd_variable init_palette(char* background, char* face_file);
void setFrame(int x, int y, unsigned short brush_color, int radius);

void close_LCD();

#endif
