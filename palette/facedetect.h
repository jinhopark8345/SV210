#ifndef FACEDETECT_H
#define FACEDETECT_H

#include "cv.h"
#include "highgui.h"

void cvIMG2RGB565(IplImage *img, unsigned short *cv_rgb, int ex, int ey);
void Fill_Background(unsigned short color);
void RGB2cvIMG(IplImage *img, unsigned short *rgb, int ex, int ey);
int detect_and_draw(IplImage *img);
int init_facedetect();
int close_facedetect();

int fb_display(unsigned short *rgb, int sx, int sy);

#endif
