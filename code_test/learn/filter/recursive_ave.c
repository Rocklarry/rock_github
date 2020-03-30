/*************************************************************************
	> File Name: recursive_ave.c
	> Author: Rock
	> Mail: ruidongren@163.com 
	> Created Time: 2017年03月24日 星期五 11时06分42秒
 ************************************************************************/

#include<stdio.h>


int Filter(int); 


int main()
{
int foo;
//while(1)
//{
foo = Filter(3);
foo = Filter(6);
foo = Filter(9);
foo = Filter(3);
foo = Filter(6);
foo = Filter(9);
foo = Filter(3);
foo = Filter(6);
foo = Filter(9);
foo = Filter(3);
foo = Filter(6);
foo = Filter(9);
foo = Filter(3);
foo = Filter(6);
foo = Filter(9);
foo = Filter(3);
foo = Filter(6);
foo = Filter(9);
foo = Filter(3);
foo = Filter(6);
foo = Filter(9);
foo = Filter(3);
foo = Filter(6);
foo = Filter(9);
foo = Filter(3);
foo = Filter(6);
foo = Filter(9);
foo = Filter(3);
foo = Filter(6);
foo = Filter(9);
foo = Filter(3);
foo = Filter(6);
foo = Filter(9);
foo = Filter(3);
foo = Filter(6);
foo = Filter(9);
//printf("the Filter is %d\n",foo);
//}
return 0;
}


int Get_AD(void)
{
int foo;
scanf("%d",&foo);
return foo;
}


// 递推平均滤波法（又称滑动平均滤波法）
#define FILTER_N 6
int filter_buf[FILTER_N + 1];

int Filter( int data) 
{
int i;
int filter_sum = 0;
//filter_buf[FILTER_N] = Get_AD(); //AD转换的值赋给数组最后一个值
filter_buf[FILTER_N] = data;
for(i = 0; i < FILTER_N; i++) 
{
filter_buf[i] = filter_buf[i + 1]; // 所有数据左移，低位仍掉
filter_sum += filter_buf[i];
}
printf("the Filter is %d\n",(int)(filter_sum / FILTER_N));
return (int)(filter_sum / FILTER_N);
}

