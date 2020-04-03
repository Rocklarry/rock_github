/*************************************************************************
	> File Name: main.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2020年04月01日 星期三 16时12分36秒
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h> 

#define DEVNAME "/dev/simple"

#define cmd_0	0
#define cmd_1	1
#define cmd_2	2
#define cmd_3	3

int main()
{
  char alpha[27];
  int fd,i;

  memset(alpha, 0, 27);
  for(i = 0; i < 26; i++)
	alpha[i] = 'A' + i;

  fd = open(DEVNAME, O_RDWR);
  if(fd == -1)
	printf("file %s is opening......failure!", DEVNAME);
  else
	printf("file %s is opening......successfully!\nits fd is %d\n", DEVNAME, fd);

  getchar();
  printf("write A-Z to kernel......\n");
  write(fd, alpha, 26);
	
  memset(alpha,0,27);
  getchar();
  printf("read datas from kernel.......\n");
  read(fd, alpha, 26);
  printf("%s\n", alpha);

	 getchar();//回车
	if(ioctl(fd,cmd_0)<0)
		 printf("cmd_0 faile \n");
	if(ioctl(fd,cmd_1)<0)
		 printf("cmd_1 faile \n");


  getchar();
  close(fd);
  return 0;  
}
