/*************************************************************************
	> File Name: map_test1.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2020年06月12日 星期五 14时32分07秒
 ************************************************************************/
#include<stdio.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<string.h>


typedef struct {	//共享内存区的数据往往是有固定格式的
	char name[4];
	int age;
}people;

void main(int argc,char **argv)
{
	int fd, i;
	people *p_map;
	char temp;
	fd = open(argv[1],O_CREAT|O_RDWR|O_TRUNC,00777);//打开或创建一个文件
	lseek(fd,sizeof(people)*5-1,SEEK_SET);
	write(fd,"",1);
	p_map=(people*)mmap(NULL,sizeof(people)*10,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	close(fd);
	temp='a';
	for(i=0;i<50;i++)
	{
		temp+=1;
		memcpy((*(p_map+i)).name,&temp,2);
		(*(p_map+i)).age=20+i;
		sleep(1);
	}
	printf("initializeover\n");
	sleep(10);
	munmap(p_map,sizeof(people)*10);
	printf("umapok\n");
}
