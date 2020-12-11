#ifndef CAMERA_H
#define CAMERA_H

#define CAMERA_DEVICE_PATH	"/dev/camera"
unsigned short cis_rgb[320*240*2]; // contact image censor
int camera_fd;


void init_camera();
void read_camera2rgb();
void close_camera();


#endif
