/*************************************************************************
	> File Name: pthread_lock.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2017年07月28日 星期五 19时57分12秒
 ************************************************************************/

#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>

static int value = 0;
pthread_mutex_t mutex;

void func(void * args)
{
	while(1)
	{
		pthread_mutex_lock(&mutex);
		sleep(1);
		value ++;
		printf("value =%d!\n",value);
		pthread_mutex_unlock(&mutex);
	}
}

int main()
{
	pthread_t pid1,pid2;
	pthread_mutex_init(&mutex,NULL);

	if(pthread_create(&pid1,NULL,func,NULL))
	{
		return -1;
	}

	if(pthread_create(&pid2,NULL,func,NULL))
	{
		return -1;
	}

	while(1)
		sleep(0);

	return 0;
}
