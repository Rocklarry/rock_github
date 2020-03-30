
当前目录介绍：

一、在drv目录下是这个模块的驱动代码，本驱动支持linux内核2.6及以上版本；

二、demo目录下是这个模块的demo代码，目前向用户演示如何通过用户程序，实现数据收发功能。


具体使用方法：

一、用户的"编/解码板"和酷芯的"发送/接收模块"通过usb连接线进行连接，用户"编/解码板"的usb做host，酷芯"发送/接收模块"的usb做device，用户的编码板连接酷芯的发送模块，用户的解码板连接酷芯的接收模块。

二、将驱动artosyn_usb_8020.ko（drv目录下有介绍如何编译生成）拷贝到"编/解码板"上，在"编/解码板"上，通过 "insmod artosyn_usb_8020.ko" 进行加载,加载成功后，会在/dev目录下生成artosyn_port0,artosyn_port1,artosyn_port2,artosyn_port3设备节点，此时驱动加载正常。
注意：加载驱动时，确保"编/解码板"和"发送/接收模块"处于连接状态，否则不会有对应设备节点生成。

三、编码板上artosyn_usb_8020.ko驱动加载成功后，将软件包里的demo/send 文件夹里的 usb_send_test（demo/send 目录下有介绍如何编译生成）拷贝到编码板的根目录下，将软件包里的demo/send filetosend文件拷贝到编码板的根目录下，在编码板的根目录下执行./usb_send_test来开启发送数据功能。

四、解码板上artosyn_usb_8020.ko驱动加载成功后，将软件包里的demo/receive 文件夹里的 usb_rec_test（demo/receive 目录下有介绍如何编译生成）拷贝到解码板的的根目录下，在解码板的根目录下执行touch filetorec,filetorec文件用做存储接收板接收到的数据，然后执行./usb_rec_test来开启接收数据功能，