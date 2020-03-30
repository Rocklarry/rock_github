/*************************************************************************
	> File Name: man_test.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2017年10月20日 星期五 14时02分43秒
 ************************************************************************/

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
 

void snprintf_test(void);
void strncpy_test(void);

int main(void)
{  
  
	snprintf_test();
	strncpy_test();
  
    return 0;  
}  

void strncpy_test(void)
{
	printf("\n\n /************* strncpy test **************/\n");
	char buf[5];
    char src[10] = "12345678";
    strncpy(buf, src, sizeof(buf));
    printf("%s\n",buf);
}

void snprintf_test(void)
{
	printf("\n\n /************* snprintf test **************/\n");
	char dest_str[4096];  
    memset(dest_str,0,sizeof(dest_str));  
  
    char *s1 = "Linux程序设计";  
    int size = strlen(s1);  
    int year = 2014;  
    int month = 11;  
    int day = 6;  
  
    snprintf(dest_str,sizeof(dest_str),"字符串:%s\n长度是:%d\n今天是:%d年%d月%d日\n",s1,size,year,month,day);  
  
    printf("%s",dest_str);  

}


