/*************************************************************************
	> File Name: map_test2.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2020年06月12日 星期五 14时39分03秒
 ************************************************************************/

#include<stdio.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<unistd.h>

typedef struct{
	char name[4];
	int age;
}people;

void main(int argc,char **argv)//map a normal file as shared mem:
{
	int fd,i;
	people *p_map;
	fd=open(argv[1],O_CREAT|O_RDWR,00777);
	p_map=(people*)mmap(NULL,sizeof(people)*10,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	for(i=0;i<50;i++)
	{
	printf("name:%sage%d;\n",(*(p_map+i)).name,(*(p_map+i)).age);
	}
	munmap(p_map,sizeof(people)*10);
				
}
