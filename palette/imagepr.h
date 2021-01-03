#ifndef IMAGEPR_H
#define IMAGEPR_H

#include "cv.h"
#include "highgui.h"

#define RGB565(r, g, b) ((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3))

int init_facedetect();
int close_facedetect();

void cvIMG2RGB565(IplImage *img, unsigned short *cv_rgb, int ex, int ey);

void cvIMG2GRAY(IplImage *img, unsigned short *cv_gray, int ex, int ey);

void RGB2cvIMG(IplImage *img, unsigned short *rgb, int ex, int ey);

void init_grayscale(unsigned short *cis_rgb);

int detect_face(IplImage *cv_image, unsigned short *cis_rgb,
                unsigned char *fb_mapped);

#endif
