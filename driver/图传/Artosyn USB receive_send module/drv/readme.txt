文件介绍：

    arotsyn_usb_8020.c为酷芯“发送/接收模块”的驱动源码（基于linux2.6及以上内核）


驱动编译方法：

    1、将arotsyn_usb_8020.c拷贝到内核目录下的drivers/usb/目录下

    2、修改drivers/usb/目录下的Makefile，在Makefile中的末尾加入obj-m += artosyn_usb_8020.o

    3、编译内核（内核版本需要是linux 2.6及linux 2.6以上），
       编译完成后，会在drivers/usb/目录下生成arotsyn_usb_8020.ko文件

