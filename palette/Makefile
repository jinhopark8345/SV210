# CC = arm-linux-gcc
<<<<<<< HEAD
CC=/home/dlt8730/Desktop/working/arm-2009q3/bin/arm-linux-gcc-4.4.1
=======
CC=/home/jp/Project/sv210/arm-2009q3/bin/arm-linux-gcc-4.4.1
>>>>>>> 9905fd0efd30ceda987234beb17e5e10293133cb

APP = stickerphoto
KEYBOARD = keyboard
TOUCHLCD = touchlcd
FACEDETECT = facedetect
CAMERA = camera
<<<<<<< HEAD
TEXTLCD = textlcd
SEGMENT = segment

CFLAGS = -DNO_DEBUG -I/home/dlt8730/embbed_system/opencv_test/OpenCV_1_1_0_arm/include/opencv/ -L/home/dlt8730/embbed_system/opencv_test/OpenCV_1_1_0_arm/lib/ -lcxcore -lcv -lhighgui -lcvaux -lml
=======


CFLAGS = -DNO_DEBUG -I/home/jp/Project/opencv_arm/include/opencv/ -L/home/jp/Project/opencv_arm/lib/ -lcxcore -lcv -lhighgui -lcvaux -lml
>>>>>>> 9905fd0efd30ceda987234beb17e5e10293133cb
# KDIR := /home/jp/Project/sv210/linux-2.6.32-hanback/
# CFLAGS = -DNO_DEBUG -I/usr/OpenCV_1_1_0_arm/include/opencv/ -L/usr/OpenCV_1_1_0_arm/lib/ -lcxcore -lcv -lhighgui -lcvaux -lml
# CFLAGS = -DNO_DEBUG -I/home/jp/Project/opencv_arm/include/opencv/ -L/home/jp/Project/opencv_arm/lib/ -lcxcore -lcv -lhighgui -lcvaux -lml

TOUCH= $(TOUCHLCD).o
KEYBO= $(KEYBOARD).o
CAM= $(CAMERA).o
FACE = $(FACEDETECT).o
<<<<<<< HEAD
TEXT = $(TEXTLCD).o
SEG = $(SEGMENT).o
=======
>>>>>>> 9905fd0efd30ceda987234beb17e5e10293133cb
OBJS = $(APP).o

all: $(APP)

<<<<<<< HEAD
$(APP):$(KEYBO) $(CAM) $(SEG) $(TOUCH) $(FACE) $(TEXT) $(OBJS)
	$(CC)  $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) $(TEXT) $(SEG) $(KEYBO) $(CAM) $(TOUCH) $(FACE) $(APP)
=======
$(APP):$(KEYBO) $(CAM) $(TOUCH) $(FACE) $(OBJS)
	$(CC)  $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) $(KEYBO) $(CAM) $(TOUCH) $(FACE) $(APP)
>>>>>>> 9905fd0efd30ceda987234beb17e5e10293133cb
