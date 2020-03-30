/*************************************************************************
	> File Name: strstr.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2018年03月14日 星期三 16时39分31秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>

int  main(void)
{
	FILE *fp;
	char buf[1024];

	fp = fopen("mounts","r");
	if(fp == NULL){
		printf("fopen error \n\n");
		return 0;
	}

	while(!feof(fp)){
		fgets(buf,128,fp);

		printf("but =%s \n",buf);
		if(strstr(buf, "/mnt/sdcard")) {
			printf("\n\n\n  mnt  open \n\n\n");
				
		}else{
			printf("no no no \n");
		}
	}
	
		return 0;
}
