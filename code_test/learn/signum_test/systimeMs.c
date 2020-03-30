#include <stdio.h>
#include <time.h>
typedef signed long long     int64_t;
typedef long long OMX_S64; 
typedef OMX_S64 OMX_TICKS;
OMX_TICKS mStartMs;

static int64_t getSystemMs(){
    struct timespec t;

    clock_gettime(CLOCK_BOOTTIME, &t);
    return t.tv_sec*1000 + t.tv_nsec/1000000;
}
int main(void)
{
	mStartMs = getSystemMs();
	printf("\n\n*****************mStartMs = %d **********\n\n",mStartMs);
	return 0;
}