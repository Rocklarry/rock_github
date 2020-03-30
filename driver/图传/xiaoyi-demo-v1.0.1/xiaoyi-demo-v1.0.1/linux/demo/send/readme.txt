
文件介绍：

test_usb_send.c  --------  用户发送端的demo程序源码，用于演示如何操纵发送端口，进行数据发送
usb_send_build   --------  demo程序的编译脚本，用户通过执行./usb_send_build来进行编译
usb_send_test    --------  编译后生成的可执行文件
filetosend       --------  h264的视频文件，演示发送文件时用到


源码说明：

1、驱动提供给用户的发射端口是/dev/artosyn_port1,

2、test_usb_send.c程序只是向用户演示如何通过发送端口，进行数据发送，逻辑比较简单，用户在正常使用中，可以做一些调整，
如：开启一个发送线程，专门用做发送，或数据交互采用环形buf方式等，

3、驱动在发送端，做了限制包大小逻辑，一次最大能发送2048个字节，用户一次写超过2048，则write函数返回2048个字节，
如果一次写的length小于2048个字节，write函数会返回length个字节


注意：

因为用户的平台差异，在执行./usb_send_build之前，需要修改usb_send_build脚本，将脚本中的
/opt/freescale/usr/local/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/fsl-linaro-toolchain/bin/arm-none-linux-gnueabi-gcc
替换成用户的交叉编译工具。