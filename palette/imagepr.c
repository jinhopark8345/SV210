
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
#include "imagepr.h"

#define RGB565(r,g,b)	((((r)>>3)<<11) | (((g)>>2)<<5) | ((b)>>3))
#define SAVE_FILE_NAME	"face_image.jpg"

static CvMemStorage* storage = 0;
static CvHaarClassifierCascade* cascade = 0;
struct fb_var_screeninfo fbvar;

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


void init_grayscale(unsigned short *cis_rgb){

	IplImage *gray =cvCreateImage(cvSize(320, 240), 8, 1);
    IplImage *temp_cv_image = cvCreateImage(cvSize(320, 240), IPL_DEPTH_8U, 3);


    // convert current cis_rgb value to cv_image
    RGB2cvIMG(temp_cv_image, cis_rgb, 320, 240);

    // get grayscaled image of temp_cv_image in gray
	cvCvtColor(temp_cv_image, gray, CV_BGR2GRAY);

    // make bgr channel gray scaled image in temp_cv_image
    cvCvtColor(gray, temp_cv_image, CV_GRAY2BGR);

    // change cis_rgb value with gray scaled values(rgb)
	cvIMG2RGB565(temp_cv_image, cis_rgb, 320, 240);

	cvReleaseImage(&gray);
	cvReleaseImage(&temp_cv_image);
}



int init_facedetect(){
	int fd;
	int ret=0;
	int fbfd, mem_fd;

	cascade = (CvHaarClassifierCascade*)cvLoad(cascade_xml, 0, 0, 0);
	if(!cascade){
		fprintf(stderr, "ERROR: Could not load classifier cascade\n");
		fprintf(stderr, "Usage : ./facedetect -- cascade=[CASCADE_PATH/FILENAME]\n");
		return -1;
	}

	storage = cvCreateMemStorage(0);

	if((fd=open("/dev/mem", O_RDWR|O_SYNC)) < 0){
		perror("mem open fail\n");
		exit(1);
	}
	

	/* camera_image = cvCreateImage(cvSize(320, 240), IPL_DEPTH_8U, 3); */


	return 0;
}

int detect_face(IplImage *cv_image, unsigned short *cis_rgb, unsigned char *fb_mapped){

    int ret = 0;

	double scale = 1.3;
	CvSize x = cvSize(cvRound (cv_image->width/scale), cvRound (cv_image->height/scale));
	IplImage *gray = cvCreateImage(cvSize(cv_image->width, cv_image->height), 8, 1);
	IplImage *small_img = cvCreateImage(x, 8, 1);
    int i;

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


	cvCvtColor(cv_image, gray, CV_BGR2GRAY);
	cvResize(gray, small_img, CV_INTER_LINEAR);

	cvEqualizeHist(small_img, small_img);

	cvClearMemStorage(storage);

    // convert camera(rgb) image to cv image
    RGB2cvIMG(cv_image, cis_rgb, 320, 240);

    if(cv_image){
        if(cascade){
            /* CvSeq *faces = cvHaarDetectObjects(gray, cascade, storage, 1.1, 2, 0,cvSize(30,30)); */
            CvSeq *faces = cvHaarDetectObjects(gray, cascade, storage, 1.1, 2, 0 /*CV_HAAR_DO_CANNY_PRUNING*/, cvSize(30,30));
            for(i=0; i<(faces ? faces->total : 0); i++){
                ret++;
            }
        }
    }

	cvReleaseImage(&gray);
	cvReleaseImage(&small_img);

    return ret;
}



int close_facedetect(){

    //
	cvClearMemStorage(storage);
    // maybe clear these vars as well?
    /* static CvMemStorage* storage = 0; */
    /* static CvHaarClassifierCascade* cascade = 0; */
    /* struct fb_var_screeninfo fbvar; */

}
