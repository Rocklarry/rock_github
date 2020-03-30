/*************************************************************************
	> File Name: test.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2019年09月17日 星期二 19时24分26秒
 ************************************************************************/

 /*
  * gcc test.c -o test  -lpthread
	gcc -o avs_read_test avs_read_test.c  -lpthread
  *                                                                                                                                                 
  * 
  */


#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

char  buffer[120];
char code_null[] ="null";


char  *get_code(){	
	int fd,size;
    fd=open("code.cfg",O_RDONLY );
	if(fd >0)
	{
		size=read(fd,buffer,sizeof(buffer));
		close(fd);
		printf("%s",buffer);
		if(strlen(buffer)<2)
			return code_null;
		else
			return buffer;
	}else{
		return code_null;
	}
}

int heat_test()
{ 
	float y,x,a;
    for ( y = 1.5f; y > -1.5f; y -= 0.1f)
    {
        for ( x = -1.5f; x < 1.5f; x += 0.05f)
        {
             a = x * x + y * y - 1;
            putchar(a * a * a - x * x * y * y * y <= 0.0f ? '*' : ' ');
        }
        putchar('\n');
    }
}

void *upd_recvfrom()
{
	int i;
	for(i=0;i<20;i++){
		printf("===rock=== upd_recvfrom Starting BBBB=%d \n",i);
		usleep(500);
		}
	}

int main()
{
	int i;
	printf("===rock == send code_s  = %s \n", get_code());

	pthread_t upd_thread;
	  printf("===rock=== upd_thread Starting  \n");
  if (pthread_create(&upd_thread,NULL,(void*) upd_recvfrom,NULL)==0)
	printf("pthread_create ok \n");
	
	
	

	for(i=0;i<20;i++){
		
		printf("===rock=== upd_thread Starting   AAAA=%d \n",i);
		usleep(200);
		}

	//test();
	return 0;
}
