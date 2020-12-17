# CC = arm-linux-gcc

CC=/home/jp/Project/sv210/arm-2009q3/bin/arm-linux-gcc-4.4.1
# CC=/home/dlt8730/Desktop/working/arm-2009q3/bin/arm-linux-gcc-4.4.1
# CC=/home/dlt8730/Desktop/working/arm-2009q3/bin/arm-linux-gcc-4.4.1

APP = stickerphoto
KEYBOARD = keyboard
TOUCHLCD = touchlcd
IMAGEPR = imagepr
CAMERA = camera
TEXTLCD = textlcd
SEGMENT = segment
DOTMATRIX = dotmatrix
GPIO = gpio
KEYPAD = keypad
DIPSW = dipsw

CFLAGS = -DNO_DEBUG -I/home/jp/Project/opencv_arm/include/opencv/ -L/home/jp/Project/opencv_arm/lib/ -lcxcore -lcv -lhighgui -lcvaux -lml
# CFLAGS = -DNO_DEBUG -I/home/dlt8730/embbed_system/opencv_test/OpenCV_1_1_0_arm/include/opencv/ -L/home/dlt8730/embbed_system/opencv_test/OpenCV_1_1_0_arm/lib/ -lcxcore -lcv -lhighgui -lcvaux -lml

# KDIR := /home/jp/Project/sv210/linux-2.6.32-hanback/

TOUCH= $(TOUCHLCD).o
KEYBO= $(KEYBOARD).o
CAM= $(CAMERA).o
IMPR = $(IMAGEPR).o
TEXT = $(TEXTLCD).o
SEG = $(SEGMENT).o
DOT = $(DOTMATRIX).o
OBJS = $(APP).o
GP = $(GPIO).o
DIPS = $(DIPSW).o
KEYP = $(KEYPAD).o

all: $(APP)

$(APP):$(GP) $(DIPS) $(KEYP) $(KEYBO) $(DOT) $(CAM) $(SEG) $(TOUCH) $(IMPR) $(TEXT) $(OBJS)
	$(CC)  $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) $(DIPS) $(GP) $(KEYP) $(DOT) $(TEXT) $(SEG) $(KEYBO) $(CAM) $(TOUCH) $(IMPR) $(APP)
