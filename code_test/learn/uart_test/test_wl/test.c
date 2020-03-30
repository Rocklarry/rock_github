
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
 
 
int main()
{
 int fd;
 struct termios st;
 char ch;
 if((fd=open("/dev/ttyHSL2", O_RDWR | O_NONBLOCK))<0){
  printf("open failed\n");
  exit(1);
 }
 st.c_iflag = 0;
 st.c_oflag = 0;
 st.c_cflag = 0;CS8|CREAD|CLOCAL;
 cfsetospeed(&st, B115200);
 cfsetispeed(&st, B115200);
 tcsetattr(fd, TCSANOW, &st);
 for(;;){
  if(read(fd,&ch,1)>0){
   //write(STDOUT_FILENO,&ch,1);
   printf("%c\n",ch);
  }
 }
}
