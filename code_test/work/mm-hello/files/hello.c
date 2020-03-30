/*************************************************************************
	> File Name: tmp.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2017年01月03日 星期二 17时44分09秒
 ************************************************************************/

#include<stdio.h>  
#include <stdlib.h>  
#include <string.h>
#include <unistd.h>
#include <syslog.h>



int  main(int argc, char * argv[])  
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
		  
	printf(" \t\t\t OTA update  ok \n\n");
	
        //获取编译日期、时间
	printf("\t\t******************************************\n");
	printf("\t\t**\t\t date :  %s \t** \n",__DATE__); //"Sep 18 2010"
	printf("\t\t**\t\t time :  %s \t**\n ",__TIME__);    //"10:59:19"
	printf("\t\t******************************************\n\n\n");

	
	
	return 0;
}
