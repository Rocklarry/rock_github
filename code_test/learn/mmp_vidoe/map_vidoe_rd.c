/*************************************************************************
	> File Name: map_normalfile2.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2018年03月23日 星期五 16时51分47秒

		./map_normalfile2 test

 ************************************************************************/

#include<stdio.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


typedef struct{
  //char name[4];
  int test;
  int  age;
}people;

int main(int argc, char** argv)  // map a normal file as shared mem:
{
  int fd,i;
  people *p_map;
  fd=open( argv[1],O_CREAT|O_RDWR,00777 );
  p_map = (people*)mmap(NULL,sizeof(people),PROT_READ|PROT_WRITE,
       MAP_SHARED,fd,0);
  for(i = 0;i<500;i++)
  {
	printf( "name: %d age %d;\n",(*(p_map+i)).test, (*(p_map+i)).age );
	usleep(200*1000);
  }
  munmap( p_map,sizeof(people) );

  return 0;
 }