/*************************************************************************
	> File Name: invSqrt.c
	> Author: Rock
	> Mail: ruidongren@163.com 
	> Created Time: 2017年04月14日 星期五 13时56分19秒
 ************************************************************************/

#include<stdio.h>

float InvSqrt (float x)  
{  
	float xhalf = 0.5f*x;  
	int i = *(int*)&x;  
	i = 0x5f3759df - (i >> 1); // 计算第一个近似根  
	x = *(float*)&i;  
	x = x*(1.5f - xhalf*x*x); // 牛顿迭代法  
	return x;  
} 
   
   // 计算参数x的平方根的倒数   
   //   
   float InvSqrt_ts (float x)   
   {   
   float xhalf = 0.5f*x;   
   int i = *(int*)&x;   
   i = 0x5f3759df - (i >> 1); // 计算第一个近似根   
   x = *(float*)&i;   
   x = x*(1.5f - xhalf*x*x); // 牛顿迭代法   
   return x;   
   }  

int main(void)
{
	printf("=== %f\n",InvSqrt(3.1415));
	printf("=== %f\n",InvSqrt(4));

	float a;
	
	printf("输出一个数字:\n");
	scanf("输出一个数字：%f",a);

	printf("=== %f\n",InvSqrt(a));
	printf("=== %f\n",InvSqrt_ts(a));

	return 0;
}
