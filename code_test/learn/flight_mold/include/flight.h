#ifndef __FLIGHT_H__
#define __FLIGHT_H__
#include<string.h>    
#include<stdio.h>    
#define N 8  //    
#define N1 7  // N1为排序码的最大位数     
#define N2 97// N2为基数     
//航班信息    
//int flag1 = 0;
typedef struct flight   
{   
    char number[10];  //航班号    
    char staddress[10];   //起点站    
    char arraddress[10];   //终点站    
    char DATE[10];//班期    
    char TYPE[4]; //机型    
    int stime;     //起飞时间    
    int atime;   //到达时间    
    int value;           //票价     
}DataType;   
struct flight Flight[N];   
typedef char KeyType;   
struct Node;             //单链表结点类型    
typedef struct Node jsNode;   
struct Node   
{       
 KeyType key[N1];      
 DataType info;       
 jsNode *next;   
};   
typedef jsNode * jsList;   
typedef struct QueueNode   
{       
    jsNode *f;           //队列的头指针     
    jsNode *e;           //队列的尾指针     
}Queue;   
Queue queue[N2];   
struct Node ys[N+2];

#endif
