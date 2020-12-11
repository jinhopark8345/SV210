
# 2020.11.29, Seunghoon
please add camera.ko, textlcd.ko, segment.ko

## textlcd function added
	-you can check which mode is activated
	-you can also check color number when you are activating r,g,b mod(1~10)
	
## segment function added
	- you can check color number of r,g,b(1~10)
	- but I still don't know why 0 is not plotted on it.
	- Please, use the device driver which is loaded on the board or
      use the c file which is included in the segment folder in the palette folder.

## dotmatrix function added
	-you can check detected face number
	-use device driver on the board

## gray scale mode added
	- you can see the video and real-time gray-scaled video.
	- but I don't know how gray scale function work exactly yet.
	- you can exit this mode when you press 'q'.
## face detect mod changed
    - When you press 'q', you can exit face detect mode


# 2020.11.29, Jinho
## Start
    - move README.md to project root dir
    - add redeclaration guards to 
        - textlcd
        - segment
        - dotmatrix
### what is a redeclaration guards?
```C
// camera.h
#ifndef TEXTLCD_H
#define TEXTLCD_H

void do_something();

#endif
```
When the code is compiled, the preprocessor checks whether
HEADERFILE_H has been previously defined. If this is the first time we
have included the header, HEADERFILE_H will not have been
defined. Consequently, the compiler defines HEADERFILE_H and includes
the contents of the file.

If the header is included again into the same file, HEADERFILE_H will
already have been defined from the first time that the contents of the
header were included; the ifndef guard will then ensure that the
contents of the header will be ignored.

These header guards prevent redeclaration of any identifiers such as
types, enums, classes, and static variables. They also prevent
recursive inclusions; for example, a case where “file1.h” includes
“file2.h” and “file2.h” includes “file1.h”.
    - clean up indentation 

## modularize facedetect
    - we facedetect module should not have touchlcd or camera module if it needs, it should get as a parameter
    - init_facdetect.c: remove other device driver opening part from facedetect.c
## implement camera part
    - c : start camera
    - s : save image 
    - S : load saved image and start editing
## implement modularized facedetection 
## implement modularized grayscaling 

# 2020.12.01~03, Jinho
# modularize gpio button
# modularize keypad 
# make keypad non blocking

# 2020.12.08, SeungHun
add segment.ko dotmatrix.ko gpiobutton.ko keypad.ko camera.ko textlcd.ko dipsw.ko


edited 

## stickerphoto.c

unsigned short rgbcol; ---> unsigned int rgbcol;

changed camera load, save function key(S--->l, s--->gpio interrupt)

#define DIPSW_ON 512--->sw1이 모두 켜지면 동작

dip_read()--->for stickerphoto activation on/off

vkey check part ---> 
1. DIPSW_ON : 모든 동작을 받는다.
2. ESC입력 : 종료
3. DIPSW OFF : user_input에 'x'입력, LCD에 SET DIPSW 255출력

## facedetect.c

fb_display function added

added fb_display function to detect_and_draw_gray function
(출력이 되지않았던 이유는 이미지 처리된 결과가 LCD_print에서 fb_mapped를 가지고 출력을 하는데
detect_and_draw_gray에서 fb_mapped에 gray scaled된 이미지를 저장하지 않았음)  

## gpio.c

gpio_button variable move to gpio.h

## dipsw.c

added dipsw_read dip_init

# 2020.12.11 seunghun
keypad동작을 다음과 같이 활성화
##keypad
1. keypad의 keypad.c의
#define TIMER_INTERVAL 5--->50으로 수정(아마도 읽어들이는 타이밍을 수정하는 변수인듯)

2. palette의 keypad.c의 
......
    for (i = 0; i < 4; i++) {
    /* for (i = 0; i < 64; i++) { */
        // event type : KEY
        // event value : 0(pressed)
        if ((event_buf[i].type == EV_KEY) && (event_buf[i].value == 0)) {
            printf("\n Button key : %d, %d\n", event_buf[i].code, i);
            rtv_input = event_buf[i].code;
            rtv_input = translate_keypad(rtv_input);
            event_buf[i].value = 1;
//value가 왜인지는 모르겠지만 1이 입력되지 않는듯함
//value를 강제로 1로 써줘서 if문을 돌지않게만듦
            Ok_flag = 1; 
            if (event_buf[i].code == 26) {
                printf("\napplication : Exit Program!! (key = %d)\n", event_buf[i].code);
                /* quit = 0; */
            }
        }
    }
......

##camera
camera나 영상처리 중 종료할 때의 key를 16번(ESC, 메뉴 선택모드에서는 stickerphoto종료)로 통일
camera모드의 load모드는 keypad의 13번으로 변경

##Load image
.bmp 가아니면 load가 안되는 문제가 있음
그림판으로 그린 image도 load할 수 있게하려면 cvsetdata함수를 써야한다?
간혹 camera기능 동작시 load keypad입력후 입력이 여러개 받아들여져 카메라가 종료되어버리는 문제가 있다.
tmp_cv_img로 load하지 않고 .bmp파일을 load하는 것을 권장

