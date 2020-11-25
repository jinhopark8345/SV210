# CC = arm-linux-gcc
CC=/home/jp/Project/sv210/arm-2009q3/bin/arm-linux-gcc-4.4.1

APP = stickerphoto
KEYBOARD = keyboard

# KDIR := /home/jp/Project/sv210/linux-2.6.32-hanback/
# CFLAGS = -DNO_DEBUG -I/usr/OpenCV_1_1_0_arm/include/opencv/ -L/usr/OpenCV_1_1_0_arm/lib/ -lcxcore -lcv -lhighgui -lcvaux -lml
# CFLAGS = -DNO_DEBUG -I/home/jp/Project/opencv_arm/include/opencv/ -L/home/jp/Project/opencv_arm/lib/ -lcxcore -lcv -lhighgui -lcvaux -lml

KEYBO= $(KEYBOARD).o
OBJS = $(APP).o

all: $(APP)

$(APP):$(KEYBO) $(OBJS)
	$(CC)  $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) $(KEYBO) $(APP)
