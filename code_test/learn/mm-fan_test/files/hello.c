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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int  main(int argc, char * argv[])  
{

		 int fd;

    //char s[]="linux programmer!\n";

    char buffer[80];

	printf("\t\t******************************************\n");

   // fd=open("/tmp/temp",O_WRONLY);

   // write(fd,s,sizeof(s));

    //close(fd);

    fd=open("/sys/class/android_usb/android0/idProduct",O_RDONLY);

    read(fd,buffer,sizeof(buffer));

    close(fd);

    printf("%s",buffer);
	/*
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
	*/
        //获取编译日期、时间
	printf("\t\t******************************************\n");
	printf("\t\t**\t\t date :  %s \t** \n",__DATE__); //"Sep 18 2010"
	printf("\t\t**\t\t time :  %s \t**\n ",__TIME__);    //"10:59:19"
	printf("\t\t******************************************\n\n\n");


	//system("/usr/bin/setusbcomposition  mtp");
	
	 //execl("/bin/sh", "sh", "/usr/bin/setusbcomposition", "mass_storage", NULL);
	printf("\t\t*setusbcomposition mtp \n\n\n");




	return 0;
}
