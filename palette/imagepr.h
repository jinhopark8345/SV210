#ifndef IMAGEPR_H
#define IMAGEPR_H

#include "cv.h"
#include "highgui.h"


void cvIMG2RGB565(IplImage *img, unsigned short *cv_rgb, int ex, int ey);

void cvIMG2GRAY(IplImage *img, unsigned short *cv_gray, int ex, int ey);

void RGB2cvIMG(IplImage *img, unsigned short *rgb, int ex, int ey);

/* int detect_and_draw(IplImage *img, unsigned char *fb_mapped, unsigned short *cis_rgb); */

void init_grayscale(unsigned short *cis_rgb);


int init_facedetect();

int detect_face(IplImage *cv_image, unsigned short *cis_rgb, unsigned char *fb_mapped);
int close_facedetect();



#endif
