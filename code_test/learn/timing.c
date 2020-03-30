#include <stdio.h>        //printf()
#include <unistd.h>        //pause()
#include <signal.h>        //signal()
#include <string.h>        //memset()
#include <sys/time.h>    //struct itimerval, setitimer()
#include <limits.h>

static int count = 0;

void printMes(int signo)
{
    printf("Get a SIGALRM, %d counts!\n", ++count);
}

int main()
{
    int res = 0;
    struct itimerval tick;


	struct timeval tv;
   // gettimeofday(&tv,NULL);
    //printf("second:%ld\n",tv.tv_sec);  //秒
   // printf("millisecond:%ld\n",tv.tv_sec*1000 + tv.tv_usec/1000);  //毫秒
    //printf("microsecond:%ld\n",tv.tv_sec*1000000 + tv.tv_usec);  //微秒
	
	static long tmp;


    
    signal(SIGALRM, printMes);
    memset(&tick, 0, sizeof(tick));

    //Timeout to run first time
    tick.it_value.tv_sec = 0;
    tick.it_value.tv_usec = 5000;

    //After first, the Interval time for clock
    tick.it_interval.tv_sec = 0;
    tick.it_interval.tv_usec = 5000;

    if(setitimer(ITIMER_REAL, &tick, NULL) < 0)
            printf("Set timer failed!\n");

    //When get a SIGALRM, the main process will enter another loop for pause()



	printf("size of char %d\n",sizeof(char));
	printf("size of long %d\n",sizeof(long));
	printf("size of ulong %d\n",sizeof(unsigned long long int ));
	printf("size of int %d\n",sizeof(int ));
	printf("size of uint %d\n",sizeof(unsigned  int ));
	printf("size of double %d\n",sizeof(double));
	

    /*while(1)
    {
        pause();
		gettimeofday(&tv,NULL);
		printf("microsecond:%ld\n",(tv.tv_sec*1000000 + tv.tv_usec) -tmp);  //微秒

		tmp = tv.tv_sec*1000000 + tv.tv_usec;
    }*/
    return 0;
}
