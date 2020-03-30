#kefile for the helloworld.c
#
obj-m := artosyn_usb_8020.o
CURRENT_PATH := $(shell pwd)
LINUX_KERNEL := $(shell uname -r)
LINUX_KERNEL_PATH := /usr/src/linux-headers-$(LINUX_KERNEL)
all:
	$(MAKE) -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules
clean:
	rm *.ko
	rm *.o



export KLIB:=          /lib/modules/$(shell uname -r)
export KLIB_BUILD ?=	$(KLIB)/build
export PWD :=	$(shell pwd)
	$(MAKE) -C $(KLIB_BUILD) M=$(PWD) modules