
gcc mixer.c  pcm.c -I ./include/   -fPIC -shared -o libtinyalsa.so

gcc tinyplay.c  -I ./include/ -fpic ./libtinyalsa.so -o tinyplay

gcc tinymix.c  -I ./include/ -fPIC ./libtinyalsa.so -o tinymix

修改tinycap.c
sigint_handler(int sig )
gcc tinycap.c  -I ./include/ -fPIC ./libtinyalsa.so -o tinycap

gcc tinyhostless.c  -I ./include/ -fPIC ./libtinyalsa.so -o tinyhostless


gcc tinypcminfo.c  -I ./include/ -fPIC ./libtinyalsa.so -o tinypcminfo