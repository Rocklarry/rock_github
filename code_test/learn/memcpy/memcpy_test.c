/*************************************************************************
	> File Name: memcpy_test.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2017年09月04日 星期一 14时36分21秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(void)
{
	char *p1 = "asdfghqscvghuik";
	char *p2 = (char *)malloc(100*sizeof(char));
	//char *p3 = memcpy(p2,p1,strlen(p1));
	char *p3 = memcpy(p2,p1,10);
	printf("*p3 =%s *p2=%s\n",p3,p2);
	free(p2);
	return 0;
}
