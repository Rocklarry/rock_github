#kefile for the copy_to_from_user.c
#
obj-m := copy_to_from_user.o
CURRENT_PATH := $(shell pwd)
LINUX_KERNEL := $(shell uname -r)
#LINUX_KERNEL_PATH := /usr/src/linux-headers-$(LINUX_KERNEL)
LINUX_KERNEL_PATH := /lib/modules/4.4.0-31-generic/build
#KDIR := /home/rock/qccode/drone_source/apq8074/tmp/apq8074-le-1-2_ap_standard_oem.git/apps_proc/linux
all:
	$(MAKE) -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules
clean:
	rm *.ko
	rm *.o
