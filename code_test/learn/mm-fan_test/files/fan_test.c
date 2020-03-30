/*************************************************************************
	> File Name: fan_test.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2018年11月27日 星期二 17时40分46秒
 ************************************************************************/


#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>



void Sleep(int ms)
{
	struct timeval delay;
	delay.tv_sec = 0;
	delay.tv_usec = ms * 1; // 1 ms
	select(0, NULL, NULL, NULL, &delay);
}

void fwrite_w(char *str)
{
	FILE *fp = NULL;
	fp = fopen("/sys/class/gpio/gpio115/value", "rw+");
	fwrite(str, sizeof(char), 1, fp);
	fclose(fp);

}


//纳秒级别的延迟
int nano_delay(long delay)
{
    struct timespec req, rem;
    long nano_delay = delay;
    int ret = 0;
    while(nano_delay > 0)
    {
            rem.tv_sec = 0;
            rem.tv_nsec = 0;
            req.tv_sec = 0;
            req.tv_nsec = nano_delay;
            if(ret = (nanosleep(&req, &rem) == -1))
            {
                printf("nanosleep failed.\n");                
            }
            nano_delay = rem.tv_nsec;
    };
    return ret;
}

void* pthread_do(void *arg)
{
	long delay;
	int i;
	char on[] = "1";
	char off[] = "0";


	 for( delay = 500;delay<2500;delay++)
	{
		for( i=0;i<1;i++)
		{

		fwrite_w(on);
		nano_delay(17000*1000-delay*1000);
		fwrite_w(off);
		nano_delay((delay-200)*1000);

		}
		printf("delay %d\n",delay);
	}
	/*
	for( delay = 2500;delay>500;delay--)
	{
		for( i=0;i<1;i++)
		{

		fwrite_w(on);
		nano_delay(17000-delay);
		fwrite_w(off);
		nano_delay(delay-200);

		}
		printf("delay %d\n",delay);
	}*/
    return NULL;
}


int main(void)
{

	char buf[1];
	FILE *fp = NULL;
	int delay,i;

	char on[] = "1";
	char off[] = "0";

	pthread_t pthd;

 pthd = pthread_create(&pthd, NULL, pthread_do, NULL);

 
	 for( delay = 500;delay<2500;delay++)
	{
		for( i=0;i<1;i++)
		{

		fwrite_w(on);
		Sleep(17000-delay);
		fwrite_w(off);
		Sleep(delay-200);

		}
		printf("delay %d\n",delay);
	}

		 for( delay = 2500;delay>500;delay--)
	{
		for( i=0;i<1;i++)
		{

		fwrite_w(on);
		Sleep(17000-delay);
		fwrite_w(off);
		Sleep(delay-200);

		}
		printf("delay %d\n",delay);
	}
	//fclose(fp);
	return 0;
}
