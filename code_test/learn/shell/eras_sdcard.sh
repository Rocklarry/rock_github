#########################################################################
# File Name: del_sd.sh
# Author: rock
# mail: rock@163.com
# Created Time: 2018年03月12日 星期一 11时32分56秒
#########################################################################
#!/bin/bash


if [ -d "/media/sdcard/" ];then
	echo "SDcard exists"
	rm -rf /media/sdcard/
else
	echo "SDcard does not exists"
fi
