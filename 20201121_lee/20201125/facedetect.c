//#include "select.h"
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
//#include "test.h"
///////////////////////////////////
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <termios.h>

#include "cv.h"
#include "highgui.h"
///////////////////////////////

#include <errno.h>
#include <signal.h>
///////////////////////
#define EVENT_BUF_NUM 1
////////////////////////
#define TEXTLCD_BASE            0xbc
#define TEXTLCD_COMMAND_SET     _IOW(TEXTLCD_BASE,0,int)
#define TEXTLCD_FUNCTION_SET    _IOW(TEXTLCD_BASE,1,int)
#define TEXTLCD_DISPLAY_CONTROL _IOW(TEXTLCD_BASE,2,int)
#define TEXTLCD_CURSOR_SHIFT    _IOW(TEXTLCD_BASE,3,int)
#define TEXTLCD_ENTRY_MODE_SET  _IOW(TEXTLCD_BASE,4,int)
#define TEXTLCD_RETURN_HOME     _IOW(TEXTLCD_BASE,5,int)
#define TEXTLCD_CLEAR           _IOW(TEXTLCD_BASE,6,int)
#define TEXTLCD_DD_ADDRESS      _IOW(TEXTLCD_BASE,7,int)
#define TEXTLCD_WRITE_BYTE      _IOW(TEXTLCD_BASE,8,int)

#define RGB565(r,g,b)   ((((r)>>3)<<11) | (((g)>>2)<<5) | ((b)>>3))
#define FBDEV_FILE      "/dev/fb0"
#define CAMERA_DEVICE   "/dev/camera"
#define FILE_NAME       "face_image.jpg"


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
unsigned short button = 0;
static int gp_dev = 0;

static CvMemStorage* storage = 0;
static CvHaarClassifierCascade* cascade = 0;
struct fb_var_screeninfo fbvar;
unsigned char *pfbmap;
unsigned short cis_rgb[320*240*2];

const char *cascade_xml = "haarcascade_frontalface_alt2.xml";
#if 0
        "haarcascade_frontalface_alt.xml";
        "haarcascade_frontalface_alt2.xml";
        "haarcascade_frontalface_alt_tree.xml";
        "haarcascade_frontalface_default.xml";
        "haarcascade_fullbody.xml";
        "haarcascade_lowerbody.xml";
        "haarcascade_profileface.xml";
        "haarcascade_upperbody.xml";
#endif

static struct termios initial_settings, new_settings;
static int peek_character = -1;


void my_signal_fun(int signum)  
{  
        read(gp_dev,&button,2);
        printf("%d\n",button); 
} 


void RED_key(int text_LCD,int * quit);
void Paint_error(int text_dev, char ch);

void close_keyboard();
int kbhit();
int readch();
int fb_display(unsigned short *rgb, int sx, int sy);
void cvIMG2RGB565(IplImage *img, unsigned short *cv_rgb, int ex, int ey);
void Fill_Background(unsigned short color);
void RGB2cvIMG(IplImage *img, unsigned short *rgb, int ex, int ey);
int detect_and_draw(IplImage *img);

void face_detect(int cam_dev, IplImage *image);
void Equalize_hist(int cam_dev, IplImage *img);
void Gray_scale(int cam_dev, IplImage *img);

int main(int argc, char** argv) {
  int i, quit = 1;
  int key_fd = -1;//keypad file descripter
  size_t read_bytes; //event num
  struct input_event event_buf[EVENT_BUF_NUM]; //
  int text_dev, dot_dev, temp=0;//textLCD Dotmatrix file descripter
  int value=0;
  int paint_flag = 0;  
 
  int fbfd, mem_fd;
  int cam_dev, ret=0;
  int optlen = strlen("--cascade=");
  unsigned short ch=0;
  CvCapture *capture=0;
  IplImage *image = NULL;

  int Oflags;

	//initiate structure for textlcd command
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
  

  if((mem_fd=open("/dev/mem", O_RDWR|O_SYNC)) < 0){
         perror("mem open fail\n");
         exit(1);
  }

  if((fbfd = open(FBDEV_FILE, O_RDWR)) < 0){
          printf("Failed to open: %s\n", FBDEV_FILE);
          exit(-1);
  }
  if(ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar) < 0){
          perror("fbdev ioctl");
          exit(1);
  }
  if(fbvar.bits_per_pixel != 16){
          fprintf(stderr, "bpp is not 16\n");
          exit(1);
  }

  pfbmap = (unsigned char*)mmap(0, fbvar.xres*fbvar.yres*2, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);
  if((unsigned)pfbmap < 0){
          perror("mmap failed...");
          exit(1);
  }

  if(argc > 1 && strncmp(argv[1], "--cascade=", optlen) == 0){
          cascade_xml = argv[1] + optlen;
  }

  cascade = (CvHaarClassifierCascade*)cvLoad(cascade_xml, 0, 0, 0);
  if(!cascade){
          fprintf(stderr, "ERROR: Could not load classifier cascade\n");
          fprintf(stderr, "Usage : ./facedetect -- cascade=[CASCADE_PATH/FILENAME]\n");
          return -1;
  }

  storage = cvCreateMemStorage(0);
  Fill_Background(0x0011);

  cam_dev = open(CAMERA_DEVICE, O_RDWR);
  if(cam_dev<0){
          printf("Error: cannot open %s.\n", CAMERA_DEVICE);
          exit(1);
  }

  image = cvCreateImage(cvSize(320, 240), IPL_DEPTH_8U, 3);
//camera, opencv set
////////////////////////////////////////////////////

    signal(SIGIO, my_signal_fun);
    gp_dev = open("/dev/gpiobutton",O_RDWR);
    if(gp_dev < 0) {

          printf( "Device Open ERROR!\n");
          return -1;
    }
    printf("Please push the GPIO_0 port!\n");
    fcntl(gp_dev, F_SETOWN, getpid());  
    Oflags = fcntl(gp_dev, F_GETFL);   

//gpio sw set
///////////////////////////////////////////////////
  dot_dev = open("/dev/dotmatrix", O_WRONLY);
  text_dev = open("/dev/textlcd", O_WRONLY|O_NDELAY );
  if ((key_fd = open("/dev/input/event3", O_RDONLY)) < 0) {
    printf("application : keypad driver open fail!\n");
    exit(1);
  }
  
//dotmatrix, text lcd, key_pad set
  printf("press the key button!\n");
  printf("press the RED_key to exit!\n");
/////////////////////////////////////////////////////
  while (quit) {
	read_bytes=
        read(key_fd, event_buf, (sizeof(struct input_event) * EVENT_BUF_NUM));

    if (read_bytes < sizeof(struct input_event)) {
      printf("application : read error!!");
      exit(1);
    }

    for (i = 0; i < (read_bytes / sizeof(struct input_event)); i++) {

      // event type : KEY
      // event value : 0(pressed)
	if ((event_buf[i].type == EV_KEY) && (event_buf[i].value == 0)) {
		//delay???????????????????? chattering
		value = event_buf[i].code;
		write(text_dev, &value, 1);  
	}
	switch(value){
//Only R, face_detect, Equalize, Gray scale, paint mode can be selected for now
		case 1 : //RED
			if(paint_flag){
				printf("RED KEY DETECTED ");//DEBUGGING
				RED_key(text_dev,&quit);}
				//code here!!!
			else{
				printf("RED KEY NOTDETECTED ");//DEBUGGING
                                Paint_error(text_dev, 'P');
			}
			break;
/*		case 2 : //GREEN
			if(paint_flag){
				printf("GREEN KEY DETECTED ");//DEBUGGIGIN
			}
			break;
		case 3 : //BLUE
			printf("BLUE KEY DETECTED ");//DEBUGGING
			
			break;
*/		case 4 : //ERASER
			printf("ERASE KEY DETECTED ");//DEBUGGING
			
			break;
		case 5 : //BRUSH SMALL
			printf("BRUSH S KEY DETECTED ");//DEBUGGING
			
			break;
		case 6 : //BRUSH BIG 
			printf("BRUSH B KEY DETECTED ");//DEBUGGING
			
			break;
		case 9 : //FACE DETECTOR
			printf("FACE DETECT KEY DETECTED ");//DEBUGGING
                        face_detect(cam_dev, image);
			break;
/*		case 10 : //BLURRING
			printf("BLUR KEY DETECTED ");//DEBUGGING
			break;*/
		case 11 : //EQULIZATION
			printf("EQ KEY DETECTED ");//DEBUGGING
			Equalize_hist(cam_dev, image);
			break;
		case 12 : //GRAY SCALE
			printf("GRAY KEY DETECTED ");//DEBUGGING
			Gray_scale(cam_dev, image);
			break;
		case 13 : //PAINT MODE, USE FLAG to LOCK 1~6
			printf("PAINT KEY DETECTED ");//DEBUGGING
			if(!paint_flag){
				paint_flag = 1;
			}else{
				paint_flag = 0;
			}//YOU CAN RESET PAINT_FLAG AS WELL
			break;
		default:
			break;
	
        }
     
    }
  }
  close(key_fd);
  close(dot_dev);
  close(text_dev);
  cvReleaseImage(&image);
  close_keyboard();
  return 0;
}




void RED_key(int text_dev, int *quit){
        char Red[16]   = "Red             ";
        ioctl(text_dev,TEXTLCD_CLEAR,strcommand,32);
        printf("RED\n");        
        strcommand.pos = 0;     
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,strcommand,32);
        write(text_dev,Red,16);
//        sleep(1);
//        quit = 1;
}



void Paint_error(int text_dev, char ch){
	char err1[16]   = "SELECT PAINT    ";
	char err2[16]   = "FIRST           ";
        sleep(1);
        ioctl(text_dev,TEXTLCD_CLEAR,strcommand,32);        
        strcommand.pos = 0;     
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,strcommand,32);
        write(text_dev,err1,16);
//      sleep(1);
//        quit = 1;	

	sleep(1);
        ioctl(text_dev,TEXTLCD_CLEAR,strcommand,32);
        strcommand.pos = 40;
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,strcommand,32);
        write(text_dev,err2,16);
//        sleep(1);
//        quit = 1;
}

void face_detect(int cam_dev, IplImage *image){
////////all video related function have this part	
	int ret = 0;
	write(cam_dev, NULL, 1);
        read(cam_dev, cis_rgb, 320*240*2);
        fb_display(cis_rgb, 40, 120);
        RGB2cvIMG(image, cis_rgb, 320, 240);
//////// 
        if(image){
        	ret = detect_and_draw(image);
        }
        if((button != 0) && (ret>0)){
                cvSaveImage(FILE_NAME, image);
                button = 0;
        }
}

void Equalize_hist(int cam_dev, IplImage *image){
	double scale = 1.3;
	CvSize x = cvSize(cvRound (image->width/scale), cvRound (image->height/scale));
	IplImage *small_img = cvCreateImage(x, 8, 1);
////////all video related function have this part
	int ret = 0;	
	write(cam_dev, NULL, 1);
        read(cam_dev, cis_rgb, 320*240*2);
	fb_display(cis_rgb, 40, 120);
	RGB2cvIMG(image, cis_rgb, 320, 240);
////////
	cvEqualizeHist(small_img, small_img);
	cvClearMemStorage(storage);
	cvIMG2RGB565(small_img, cis_rgb, image->width, image->height);
	if(image){
		fb_display(cis_rgb, 435, 120);
	}

        if(button != 0){
                cvSaveImage(FILE_NAME, small_img);
                button = 0;
        }

}

void Gray_scale(int cam_dev, IplImage *image){
	IplImage *gray = cvCreateImage(cvSize(image->width, image->height), 8, 1);
////////all video related function have this part
        int ret = 0;    
        write(cam_dev, NULL, 1);
        read(cam_dev, cis_rgb, 320*240*2);
        fb_display(cis_rgb, 40, 120);
        RGB2cvIMG(image, cis_rgb, 320, 240);
////////
        cvClearMemStorage(storage);
	cvIMG2RGB565(gray, cis_rgb, image->width, image->height);
        if(image){
                fb_display(cis_rgb, 435, 120);
        }


        if(button != 0){
                cvSaveImage(FILE_NAME, gray);
                button = 0;
        }

}


int detect_and_draw(IplImage *img){
	int ret = 0;

	static CvScalar colors[] = {
					{{0,0,255}},
					{{0,128,255}},
					{{0,255,255}},
					{{0,255,255}},
					{{255,128,0}},
					{{255,255,0}},
					{{255,0,0}},
					{{255,0,255}}
				};

	double scale = 1.3;
	CvSize x = cvSize(cvRound (img->width/scale), cvRound (img->height/scale));

	IplImage *gray = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	IplImage *small_img = cvCreateImage(x, 8, 1);
	int i;

	cvCvtColor(img, gray, CV_BGR2GRAY);

	cvResize(gray, small_img, CV_INTER_LINEAR);

	cvEqualizeHist(small_img, small_img);

	cvClearMemStorage(storage);

	if(cascade){
		double t = (double)cvGetTickCount();

		CvSeq *faces = cvHaarDetectObjects(gray, cascade, storage, 1.1, 2, 0 /*CV_HAAR_DO_CANNY_PRUNING*/, cvSize(30,30));

		for(i=0; i<(faces ? faces->total : 0); i++){
			CvRect *r = (CvRect*)cvGetSeqElem(faces, i);
			CvPoint center;
			int radius;
			
			center.x = cvRound((r->x + r->width * 0.5) * scale) - 55;
			center.y = cvRound((r->y + r->width * 0.5) * scale) - 25;
			radius = cvRound((r->width + r->height) * 0x35 * scale);
			cvCircle(img, center, radius, colors[i%8], 3, 8, 0);
			ret++;
		}
	}

	cvIMG2RGB565(img, cis_rgb, img->width, img->height);
	if(ret){

		fb_display(cis_rgb, 435, 120);
	}

	cvReleaseImage(&gray);
	cvReleaseImage(&small_img);

	return ret;
}

void cvIMG2RGB565(IplImage *img, unsigned short *cv_rgb, int ex, int ey){
	int x, y;
	unsigned char r,g,b;

	for(y = 0; y < ey; y++){
		for(x=0; x < ex; x++){
			b = (img->imageData[(y*img->widthStep)+x*3]);
			g = (img->imageData[(y*img->widthStep)+x*3+1]);
			r = (img->imageData[(y*img->widthStep)+x*3+2]);
			cv_rgb[y*320 + x] = (unsigned short)RGB565(r,g,b);
		}
	}
}

void Fill_Background(unsigned short color){
	int x,y;

	for(y=0; y<480; y++){
		for(x=0; x<800; x++){
			*(unsigned short*)(pfbmap + (x)*2 + (y)*800*2) = color;
		}
	}
}

void RGB2cvIMG(IplImage *img, unsigned short *rgb, int ex, int ey){
	int x,y;

	for(y=0; y<ey; y++){
		for(x=0; x<ex; x++){
			(img->imageData[(y*img->widthStep)+x*3]) = (rgb[y*ex+x]&0x1F)<<3;	//b
			(img->imageData[(y*img->widthStep)+x*3+1]) = ((rgb[y*ex+x]&0x07E0)>>5)<<2;	//g
			(img->imageData[(y*img->widthStep)+x*3+2]) = ((rgb[y*ex+x]&0xF800)>>11)<<3;	//r
		}
	}
}


void init_keyboard() {
	tcgetattr(0, &initial_settings);
	new_settings = initial_settings;
	new_settings.c_lflag &= ~ICANON;
	new_settings.c_lflag &= ~ECHO;
	new_settings.c_lflag &= ~ISIG;
	new_settings.c_cc[VMIN] = 1;
	new_settings.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &new_settings);
}

void close_keyboard() {
	tcsetattr(0, TCSANOW, &initial_settings);
}

int kbhit() {
	char ch;
	int nread;

	if(peek_character != -1) return 1;

	new_settings.c_cc[VMIN] = 0;
	tcsetattr(0, TCSANOW, &new_settings);
	nread = read(0, &ch, 1);
	new_settings.c_cc[VMIN] = 1;
	tcsetattr(0, TCSANOW, &new_settings);

	if(nread == 1) {
		peek_character = ch;
		return 1;
	}

	return 0;
}

int readch() {
	char ch;

	if(peek_character != -1) {
		ch = peek_character;
		peek_character = -1;
		return ch;
	}
	read(0, &ch, 1);
	return ch;
}

int fb_display(unsigned short *rgb, int sx, int sy) {
	int coor_x, coor_y;
	int screen_width;
	unsigned short *ptr;

	screen_width = fbvar.xres;

	for(coor_y = 0; coor_y < 240; coor_y++){
		ptr = (unsigned short*)pfbmap + (screen_width * sy + sx) + (screen_width * coor_y);
		for(coor_x = 0; coor_x < 320; coor_x++){
			*ptr++ = rgb[coor_x + coor_y * 320];
		}
	}
	return 0;
}
