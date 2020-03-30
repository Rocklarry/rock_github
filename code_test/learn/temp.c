/*************************************************************************
	> File Name: temp.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2017年08月03日 星期四 10时00分28秒
 ************************************************************************/

#include<stdio.h>

int main()
{

	unsigned int temp;
	int h=0x23;
	int l=0x56;

	temp = (h<<8)|l;
	//temp = h;
	//temp << 8;
	//temp+=l;
	
	printf(" 0x%x  ss",temp);

	return 0;
}
