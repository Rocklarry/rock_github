
文件介绍：

test_usb_rec.c  --------  用户发送端的demo程序源码，用于演示如何操纵接收端口，进行数据接收
usb_rec_build   --------  demo程序的编译脚本，用户通过执行./usb_rec_build来进行编译
usb_rec_test    --------  编译后生成的可执行文件


源码说明：

1、驱动提供给用户的接收端口是/dev/artosyn_port1,

2、test_usb_rec.c程序只是向用户演示如何通过接收端口，进行数据接收，驱动会开启一个接收线程，实时的接收数据，所以用户的接收程序不用担心接收的实时性，但要及时的取走驱动中的数据（如果用户read的count参数为n，read返回的值小于n，表明驱动中目前没有缓存buffer).


注意：

因为用户的平台差异，在执行./usb_rec_build之前，需要修改usb_rec_build脚本，将脚本中的
/opt/freescale/usr/local/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/fsl-linaro-toolchain/bin/arm-none-linux-gnueabi-gcc
替换成用户的交叉编译工具。