/*************************************************************************
	> File Name: test.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2016年10月27日 星期四 14时53分51秒
 ************************************************************************/

//#include <cstdio>
#include<stdio.h>
#include<syslog.h>
#include<string.h>
#include<memory.h>
#include<math.h>

#include <unistd.h>

#define DRIVER_NAME "SNAV_MAG0_HMC5883L_DRIVER"
typedef enum
{
  SN_DEBUG, SN_INFO, SN_WARN, SN_ERROR
} FlightMsgPriority;

 int snav_print_ext(FlightMsgPriority input_priority, const char* filename_ptr, int line_num, const char* fmt,...);

#define snav_debug_print(fmt, ...) { snav_print_ext(SN_DEBUG,"",0,fmt,##__VA_ARGS__); }
#define snav_info_print(fmt, ...)  { snav_print_ext(SN_INFO,"",0,fmt,##__VA_ARGS__);  }
#define snav_warn_print(fmt, ...)  { snav_print_ext(SN_WARN,"",0,fmt,##__VA_ARGS__);  }
#define snav_error_print(fmt, ...) { snav_print_ext(SN_ERROR,"",0,fmt,##__VA_ARGS__); }

void snprintf_syslog_test()
{
	
	printf("==============	teset  snprintf函数测试	=================\n\n");
	
	 char str[10]={0,};
	snprintf(str, sizeof(str), "0123456789012345678");
	printf("str=%s \n", str);
	syslog(LOG_INFO,"str=%s \n", str);

	syslog(LOG_LOCAL1|LOG_INFO,"what you what print in log %s",  str);
}


void memset_test()
{
	printf("==============	teset memset函数测试	=================\n\n");

	char buffer[] = "Hello world/n";
	printf("Buffer before memset: %s\n", buffer);
	memset(buffer, '*', strlen(buffer) );
	printf("Buffer after memset: %s\n", buffer);
}

void yu_yu_test()
{
	printf("==============	  || || || 方法测试	=================\n\n");
	
	int ret1 ,ret2,ret3;
	ret1 = 1;
	ret2 = 1;
	ret3 = 1;
	if(ret1 || ret2 || ret3)
	{
		printf(" ret sss*******\n");
	}

	printf("==============	if(0)/(1) 方法测试	=================\n\n");
	if(0){
		printf(" 0000000 \n");
	}else if(1){
		printf("111111 \n");
	}
	
}


int file_buf_test()
{
	int i;
	FILE * fp;
	char msg1[] = "Hello wood\n";
	char msg2[] = "hool \n world";
	char buf[128];
	
	if((fp = fopen("buf.txt","w")) == NULL)
	{
		perror("file open faile!!!");
		return (-1);
	}
	
	setbuf(fp,NULL);
	memset(buf,'\0',128);
	fwrite(msg1,7,1,fp);
	fseek(fp,10,SEEK_END);
	fwrite(msg2,strlen(msg2),1,fp);
	printf("test setbuf(no  buf)!  check buf.txt \n");
	printf("now buf data is: buf=%s\n",buf);
//	getchar();
	fclose(fp);
	return 0;
}


void spintf_test()
{
	while(1){
	fprintf(stderr,"\033[;\033[s");
	fprintf(stderr,"\033[47;31mhello word\033[m");
	usleep(100);
	fprintf(stderr,"\033[;\033[u");
	fprintf(stderr,"\033[;\033[K");
	usleep(200);
	}
}

int main(void)
{
	 int i2c_device = 123;

	printf(" test \t\t test \n");

	snprintf_syslog_test();
	memset_test();
	
	file_buf_test();
	
//	spintf_test();

	printf(" 1=0x%x 2=0x%x\n",0<<0,1<<0);
return 0;
}
