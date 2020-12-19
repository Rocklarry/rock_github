/*************************************************************************
	> File Name: map_normalfile1.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2018年03月23日 星期五 16时51分42秒

	./map_normalfile1 test

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


int main(int argc, char** argv) // map a normal file as shared mem:
{
  int fd,i;
  people *p_map;
  char temp;
   
  fd=open(argv[1],O_CREAT|O_RDWR|O_TRUNC,00777);
  //lseek(fd,sizeof(people)*5-1,SEEK_SET);
  write(fd,"",1);
   
  p_map = (people*) mmap( NULL,sizeof(people),PROT_READ|PROT_WRITE, MAP_SHARED,fd,0 );
  close( fd );
  temp = 'a';
  for(i=0; i<500; i++)
  {
    //temp += 1;
	temp=rand() % 26;
    //memcpy( ( *(p_map+i) ).name, &temp,2 );
	( *(p_map+i) ).test = i;
    ( *(p_map+i) ).age = 20+i;
	printf( "name: %d age %d;\n",(*(p_map+i)).test, (*(p_map+i)).age );

	usleep(200*1000);
  }
  printf(" initialize over \n ");
  munmap( p_map, sizeof(people) );
  printf( "umap ok \n" );

  return 0;
}

