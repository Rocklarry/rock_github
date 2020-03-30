#########################################################################
# File Name: build.sh
# Author: rock
# mail: rock_telp@163.com
# Created Time: Wednesday, June 19, 2019 PM03:38:42 HKT
#########################################################################
#!/bin/bash


gcc -m32 ./src/mixer.c  ./src/pcm.c -I ./include/   -fPIC -shared -o ./lib/libtinyalsa.so

gcc -m32 ./src/tinyplay.c  -I ./include/ -fpic ./lib/libtinyalsa.so -o tinyplay

gcc -m32 ./src/tinymix.c  -I ./include/ -fPIC ./lib/libtinyalsa.so -o tinymix

gcc -m32 ./src/tinycap.c  -I ./include/ -fPIC ./lib/libtinyalsa.so -o tinycap

gcc -m32 ./src/tinyhostless.c  -I ./include/ -fPIC ./lib/libtinyalsa.so -o tinyhostless

gcc -m32 ./src/tinypcminfo.c  -I ./include/ -fPIC ./lib/libtinyalsa.so -o tinypcminfo