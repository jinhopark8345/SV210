# CC = arm-linux-gcc
CC=/home/dlt8730/Desktop/working/arm-2009q3/bin/arm-linux-gcc-4.4.1

APP = stickerphoto
KEYBOARD = keyboard
TOUCHLCD = touchlcd
FACEDETECT = facedetect
CAMERA = camera
TEXTLCD = textlcd
SEGMENT = segment

CFLAGS = -DNO_DEBUG -I/home/dlt8730/embbed_system/opencv_test/OpenCV_1_1_0_arm/include/opencv/ -L/home/dlt8730/embbed_system/opencv_test/OpenCV_1_1_0_arm/lib/ -lcxcore -lcv -lhighgui -lcvaux -lml
# KDIR := /home/jp/Project/sv210/linux-2.6.32-hanback/
# CFLAGS = -DNO_DEBUG -I/usr/OpenCV_1_1_0_arm/include/opencv/ -L/usr/OpenCV_1_1_0_arm/lib/ -lcxcore -lcv -lhighgui -lcvaux -lml
# CFLAGS = -DNO_DEBUG -I/home/jp/Project/opencv_arm/include/opencv/ -L/home/jp/Project/opencv_arm/lib/ -lcxcore -lcv -lhighgui -lcvaux -lml

TOUCH= $(TOUCHLCD).o
KEYBO= $(KEYBOARD).o
CAM= $(CAMERA).o
FACE = $(FACEDETECT).o
TEXT = $(TEXTLCD).o
SEG = $(SEGMENT).o
OBJS = $(APP).o

all: $(APP)

$(APP):$(KEYBO) $(CAM) $(SEG) $(TOUCH) $(FACE) $(TEXT) $(OBJS)
	$(CC)  $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) $(TEXT) $(SEG) $(KEYBO) $(CAM) $(TOUCH) $(FACE) $(APP)
