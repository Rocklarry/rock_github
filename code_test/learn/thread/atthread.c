/*************************************************************************
	> File Name: atthread.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2018年03月08日 星期四 15时40分56秒
 ************************************************************************/

#include<stdio.h>
#include<pthread.h>

void * child_thread(void *arg)
{
	printf(" child thread run! \n");

}


int main(int argc , char *argv[])
{
	pthread_ttid;

	pthread_attr_tattr;				 

	pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    pthread_create(&tid,&attr,fn,arg);
    pthread_attr_destroy(&attr);

    sleep(1);
}

