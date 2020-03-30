#obj-m:=0-chain0.o
#obj-m:=1-chain1.o 
obj-m:=2-chain2.o
mymodule-objs:=module  
KDIR:=/lib/modules/$(shell uname -r)/build  
MAKE:=make  
default:  
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules  
clean:  
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
