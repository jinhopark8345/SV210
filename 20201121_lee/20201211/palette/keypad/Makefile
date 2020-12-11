CC = /home/dlt8730/Desktop/working/arm-2009q3/bin/arm-linux-gcc-4.4.1
obj-m := keypad.o
KDIR := /home/dlt8730/Desktop/working/linux-2.6.32-hanback

PWD := $(shell pwd)

TEST_TARGET = key_test
TEST_SRCS = $(TEST_TARGET).c

default:$(TEST_TARGET)
		$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
		$(CC) $(TEST_SRCS) -o $(TEST_TARGET)
		rm -f default

clean:
		rm -f *.ko
		rm -f *.o
		rm -f *.mod.*
		rm -f .*.cmd
		rm -rf $(TEST_TARGET) Module.* module* .tmp_versions
