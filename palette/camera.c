
#include <fcntl.h>    /* for O_RDONLY */
#include <linux/fb.h> /* for fb_var_screeninfo, FBIOGET_VSCREENINFO*/
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h> /* for mmap */
#include <sys/mman.h>
#include <termios.h>
#include <unistd.h>

#include "camera.h"

void init_camera() {
  camera_fd = open(CAMERA_DEVICE_PATH, O_RDWR);
  if (camera_fd < 0) {
    printf("Error: cannot open %s.\n", CAMERA_DEVICE_PATH);
    exit(1);
  }
}

void read_camera2rgb() {
  write(camera_fd, NULL, 1);
  read(camera_fd, cis_rgb, 320 * 240 * 2);
}

void close_camera() {
  close(camera_fd); // 파일을 닫는다
}

// maybe I have to rename camera write, read function from the device drivier c
// file
