
编译找不到头文件
sudo cp -a /usr/lib/x86_64-linux-gnu/dbus-1.0/include/dbus/dbus-arch-deps.h  /usr/include/dbus/
sudo cp -a /usr/include/dbus-1.0/dbus/ /usr/include/

编译命令：
gcc -o dbus_test dbus_test.c  -l dbus-1





执行下面的命令

dbus-daemon --config-file=/home/renruidong/work/test/dbus/debug-allow-all.conf	--fork --print-address
dbus-daemon --config-file=/path/to/debug-allow-all.conf				--fork --print-address



处理 错误 Connection Err : Unable to autolaunch a dbus-daemon without a $DISPLAY for X11
命令 ：eval `dbus-launch --sh-syntax`


./dbus_test receive

./dbus_test send SIGNAL STRING hello
./dbus_test send METHOD INT32 99
