/*************************************************************************
	> File Name: imu.cpp
	> Author: Rock
	> Mail: ruidongren@163.com 
	> Created Time: 2017年05月08日 星期一 09时55分44秒
 ************************************************************************/

#include<iostream>
#include<stdio.h>

using namespace std;

#define ROW 3  
#define COL 3  
  
//矩阵顺时针旋转90度  
void Rotation(int arr[ROW][COL])  
{  
    int tmp[COL][ROW];//局部变量，函数调用完后会自动释放  
    int dst=ROW-1;    //这里我们从目标矩阵的最后一列开始存放数据  
  
    //顺时针旋转矩阵90度  
    for(int i=0;i<ROW;i++,dst--)  
        for(int j=0;j<COL;j++)  
            tmp[j][dst]=arr[i][j];  
  
    //将旋转后的矩阵保存回原来的矩阵  
    for(int i=0;i<COL;i++)  
        for(int j=0;j<ROW;j++)  
            arr[i][j]=tmp[i][j];  
}  
  
  
int main(void)  
{  
    int arr[ROW][COL]={ {0,1,0},{1,0,0},{0,0,-1}};  
    cout<<"矩阵顺时针旋转前\n";  
    for(int i=0;i<ROW;i++)  
    {  
        for(int j=0;j<COL;j++)  
            printf("%3d ",arr[i][j]);  
        cout<<endl;  
    }  
  
    Rotation(arr);  
  
    cout<<"\n矩阵顺时针旋转90度后\n";  
    for(int i=0;i<COL;i++)  
    {  
        for(int j=0;j<ROW;j++)  
            printf("%3d ",arr[i][j]);  
        cout<<endl;  
    }  
  
    cin.get();  
}  
