
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include <fcntl.h>


#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <termios.h>

#include "camera.h" // need to use read_camera2rgb function from camera module
#include "facedetect.h"

#define RGB565(r,g,b)	((((r)>>3)<<11) | (((g)>>2)<<5) | ((b)>>3))
<<<<<<< HEAD
#define FBDEV_FILE      "/dev/fb0"
#define CAMERA_DEVICE   "/dev/camera"
=======
>>>>>>> 9905fd0efd30ceda987234beb17e5e10293133cb
#define SAVE_FILE_NAME	"face_image.jpg"

static CvMemStorage* storage = 0;
static CvHaarClassifierCascade* cascade = 0;
struct fb_var_screeninfo fbvar;
IplImage *camera_image = NULL;

<<<<<<< HEAD
=======

>>>>>>> 9905fd0efd30ceda987234beb17e5e10293133cb
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


int fb_display(unsigned char *fb_mapped, unsigned short *rgb, int sx, int sy) {
	int coor_x, coor_y;
	int screen_width;
	unsigned short *ptr;

	screen_width = fbvar.xres;

    // make facedetect alone?
    // remove all touchlcd control part
    // I should remove pfbmap from below and connect this with
    // touchlcd or something or pass the parameter from here? or
    // remove fb_display function cuz it's not supposed to be in
    // facedetct
	for(coor_y = 0; coor_y < 240; coor_y++){
		ptr = (unsigned short*)fb_mapped + (screen_width * sy + sx) + (screen_width * coor_y);
		for(coor_x = 0; coor_x < 320; coor_x++){
			*ptr++ = rgb[coor_x + coor_y * 320];
		}
	}
	return 0;
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

<<<<<<< HEAD
void cvIMG2GRAY(IplImage *img, unsigned short *cv_gray, int ex, int ey){
        int x, y;
        unsigned char r,g,b;
//	unsigned char value;
	
        for(y = 0; y < ey; y++){
//I don't know why this part work well yet.
                for(x=0; x < ex; x++){
                        b = (img->imageData[(y*img->widthStep)+x]);
                        g = (img->imageData[(y*img->widthStep)+x+1]);
                        r = (img->imageData[(y*img->widthStep)+x+2]);
                        cv_gray[y*320 + x] = (unsigned short)RGB565(r,g,b);
		//	value = img->imageData[x+y*img->widthStep];
		//	cv_gray[y*320 + x] = value;
                }
        }
}


=======
>>>>>>> 9905fd0efd30ceda987234beb17e5e10293133cb
void Fill_Background(unsigned char* fb_mapped, unsigned short color){
	int x,y;

	for(y=0; y<480; y++){
		for(x=0; x<800; x++){
			*(unsigned short*)(fb_mapped + (x)*2 + (y)*800*2) = color;
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


int detect_and_draw(IplImage *img, unsigned char *fb_mapped, unsigned short *cis_rgb){
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
<<<<<<< HEAD
=======

>>>>>>> 9905fd0efd30ceda987234beb17e5e10293133cb
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
			center.y = cvRound((r->y + r->height * 0.5) * scale) - 25;
			// radius = cvRound((r->width + r->height) * 0x35 * scale);
<<<<<<< HEAD
            		radius = 60;

            		printf("radius: %d", radius);
=======
            radius = 60;

            printf("radius: %d", radius);
>>>>>>> 9905fd0efd30ceda987234beb17e5e10293133cb
			// radius = cvRound((r->width + r->height) * 0x35 * scale);
			cvCircle(img, center, radius, colors[i%8], 3, 8, 0);
			ret++;
		}
	}

	cvIMG2RGB565(img, cis_rgb, img->width, img->height);
<<<<<<< HEAD
	 if(ret){ 
		fb_display(fb_mapped, cis_rgb, 435, 120); 
	} 
=======
	/* if(ret){ */
	/* 	fb_display(fb_mapped, cis_rgb, 435, 120); */
	/* } */
>>>>>>> 9905fd0efd30ceda987234beb17e5e10293133cb

	cvReleaseImage(&gray);
	cvReleaseImage(&small_img);

	return ret;
}

<<<<<<< HEAD

void detect_and_draw_gray(IplImage *img, unsigned char *fb_mapped, unsigned short *cis_rgb){
	double scale = 1.3;
	CvSize x = cvSize(cvRound (img->width/scale), cvRound (img->height/scale));
	IplImage *gray_scale =cvCreateImage(cvSize(img->width, img->height), 8, 1);
	int i;
	cvCvtColor(img, gray_scale, CV_BGR2GRAY);//use gray_scale for output of gray_scale mode
	cvClearMemStorage(storage);

	cvIMG2GRAY(gray_scale, cis_rgb, img->width, img->height);	
	fb_display(fb_mapped, cis_rgb, 435, 120); 


	cvReleaseImage(&gray_scale);
}



int init_facedetect(unsigned char *fb_mapped){
	int fd;
	int ret=0;
	int fbfd, mem_fd;
	
	
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

	fb_mapped = (unsigned char*)mmap(0, fbvar.xres*fbvar.yres*2, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);
	if((unsigned)fb_mapped < 0){
        	perror("mmap failed...");       
		exit(1);
	}

=======
int init_facedetect(){
	int fd;
	int ret=0;
>>>>>>> 9905fd0efd30ceda987234beb17e5e10293133cb

	cascade = (CvHaarClassifierCascade*)cvLoad(cascade_xml, 0, 0, 0);
	if(!cascade){
		fprintf(stderr, "ERROR: Could not load classifier cascade\n");
		fprintf(stderr, "Usage : ./facedetect -- cascade=[CASCADE_PATH/FILENAME]\n");
		return -1;
	}

	storage = cvCreateMemStorage(0);
	/* Fill_Background(fb_mapped, 0x0011); */

	if((fd=open("/dev/mem", O_RDWR|O_SYNC)) < 0){
		perror("mem open fail\n");
		exit(1);
	}
<<<<<<< HEAD
	

=======
>>>>>>> 9905fd0efd30ceda987234beb17e5e10293133cb
	camera_image = cvCreateImage(cvSize(320, 240), IPL_DEPTH_8U, 3);


	return 0;
}

<<<<<<< HEAD
int detect_face(unsigned short *cis_rgb, unsigned char *fb_mapped,unsigned short keyboard_input){
=======
int detect_face(unsigned short *cis_rgb, unsigned char *fb_mapped){
>>>>>>> 9905fd0efd30ceda987234beb17e5e10293133cb

    int ret = 0;

    // while loop start
    read_camera2rgb();
    /* write(camera_fd, NULL, 1); */
    /* read(camera_fd, cis_rgb, 320*240*2); // read camera_fd and write it on cis_rgb */

<<<<<<< HEAD
    fb_display(fb_mapped, cis_rgb, 40, 120);

    RGB2cvIMG(camera_image, cis_rgb, 320, 240);
    if(camera_image && (keyboard_input == 'f')){
//Depend on what keyboard did you press, function is different.
//	if(ret == 0){
            ret = detect_and_draw(camera_image, fb_mapped, cis_rgb);
       // }
        if(ret>0){ 
             cvSaveImage(SAVE_FILE_NAME,camera_image);  
	} 
    }else if(camera_image && (keyboard_input == 'y')){
            detect_and_draw_gray(camera_image, fb_mapped, cis_rgb);
=======
    /* fb_display(fb_mapped, cis_rgb, 40, 120); */

    RGB2cvIMG(camera_image, cis_rgb, 320, 240);
    if(camera_image){
        while(ret == 0){
            ret = detect_and_draw(camera_image, fb_mapped, cis_rgb);
        }
        /* if(ret>0){ */
        /*     cvSaveImage(SAVE_FILE_NAME,camera_image); */
        /* } */
>>>>>>> 9905fd0efd30ceda987234beb17e5e10293133cb
    }
    // while loop end
    return ret;
}



int close_facedetect(){
	cvReleaseImage(&camera_image);
}
