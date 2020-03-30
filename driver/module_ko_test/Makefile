#kefile for the helloworld.c
#
obj-m := bmp_280.o
CURRENT_PATH := $(shell pwd)
LINUX_KERNEL := $(shell uname -r)
LINUX_KERNEL_PATH := /usr/src/linux-headers-$(LINUX_KERNEL)
#LINUX_KERNEL_PATH := /lib/modules/3.13.0-24-generic/build
#KDIR := /home/rock/qccode/drone_source/apq8074/tmp/apq8074-le-1-2_ap_standard_oem.git/apps_proc/linux
all:
	$(MAKE) -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules
clean:
	rm *.ko
	rm *.o
