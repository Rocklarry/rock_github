/*************************************************************************
	> File Name: display_time.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2020年01月18日 星期六 10时17分06秒
 ************************************************************************/

#include <stdio.h>
#include <time.h> //for sleep() function
#include <unistd.h>
#include <stdlib.h>
 
int main()
{
    int hour, minute, second;
     
    hour=minute=second=0;
 
    while(1)
    {
        //clear output screen
        system("clear");
         
        //print time in HH : MM : SS format
        printf("%02d : %02d : %02d ",hour,minute,second);
         
         //clear output buffer in gcc
        fflush(stdout);
         
         //increase second
        second++;


        //update hour, minute and second
		
		
        if(second==60){
            minute+=1;
            second=0;
        }
        if(minute==60){
            hour+=1;
            minute=0;
        }
        if(hour==24){
            hour=0;
            minute=0;
            second=0;
        }
         
        usleep(500*1000);   //wait till 1 second
    }
 
    return 0;
}