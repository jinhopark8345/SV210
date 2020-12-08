#ifndef FACEDETECT_H
#define FACEDETECT_H

#include "cv.h"
#include "highgui.h"


void cvIMG2RGB565(IplImage *img, unsigned short *cv_rgb, int ex, int ey);
void cvIMG2RGB565(IplImage *img, unsigned short *cv_gray, int ex, int ey);
void Fill_Background(unsigned char* fb_mapped, unsigned short color);
void RGB2cvIMG(IplImage *img, unsigned short *rgb, int ex, int ey);

int detect_and_draw(IplImage *img, unsigned char *fb_mapped, unsigned short *cis_rgb);

void detect_and_draw_gray(IplImage *img, unsigned char *fb_mapped, unsigned short *cis_rgb);


int init_facedetect(unsigned char *fb_mapped);

int detect_face(IplImage *cv_image, unsigned short *cis_rgb, unsigned char *fb_mapped);
int close_facedetect();


int fb_display(unsigned char *fb_mapped, unsigned short *rgb, int sx, int sy);

#endif
