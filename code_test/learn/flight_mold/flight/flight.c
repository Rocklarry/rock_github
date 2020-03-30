#include "../include/flight.h"
//基数排序 
void jsSort(jsList * plist, int d, int r)   
{   
    int i,j,k;    
    jsNode *p, *head;    
    head=(*plist)->next;   
    for(j=d-1; j>=0; j--)       
    {   
        p=head;   
        for(i=0; i<r; i++)   
        {    
            queue[i].f=NULL;  queue[i].e=NULL;    //清空队列   
        }   
        while(p!=NULL)   
        {    
            k=p->key[j];       //按排序码的第j个分量进行分配   
            if(queue[k].f==NULL)     queue[k].f=p; // 若第k个堆为空，则当前记录为队头   
            else  (queue[k].e)->next=p;         // 当前记录链接到第k队的队尾 
            queue[k].e=p;   
            p=p->next;   
        }   
        i=0;   
        while(queue[i].f==NULL)  i++;          
        p=queue[i].e;  head=queue[i].f;      //head为链表的头指针     
        for(i++; i<r; i++)   
            if(queue[i].f!=NULL)   
            {  p->next=queue[i].f;  p=queue[i].e;  }     
        p->next=NULL;   
    }   
    (*plist)->next=head;   
}   
//显示信息   
    void info()   
{   
    printf("No  staddr  arraddr  DATE  stime  atime  TYPE  value\n");    
}   
void Print(struct flight F[],int i)//打印出搜索信息  
{   
		printf(" %s",F[i].number);
		printf(" %s",F[i].staddress);
		printf(" %s",F[i].arraddress);
		printf(" %s",F[i].DATE);
		printf(" %d",F[i].stime);
		printf(" %d",F[i].atime);
		printf(" %s",F[i].TYPE);
		printf(" %d RMB",F[i].value);
		printf("\n");
}   
//将排好的链表转化成顺序表存储形式    
void copy(struct flight F[],struct Node ys[])   
{   
    jsList p=ys;   
    p=p->next;   
    int i;   
    for(i=0;i<N && p!=NULL;i++)    
    {   
        strcpy(F[i].number,p->info.number);   
        F[i].stime=p->info.stime;   
        F[i].atime=p->info.atime;   
        strcpy(F[i].staddress,p->info.staddress);   
        strcpy(F[i].arraddress,p->info.arraddress);   
        strcpy(F[i].DATE,p->info.DATE);   
        strcpy(F[i].TYPE,p->info.TYPE);   
        F[i].value=p->info.value;   
        p=p->next;   
    }   
}   
//打印现有航班表  
void ALL(struct Node ys[])    
{   
    jsList p=ys;   
    info();   
    p=p->next;   
    while(p!=NULL)   
    {   
    	  printf("%s",p->info.number);
		  printf("   %s",p->info.staddress);
		  printf("   %s",p->info.arraddress);
		  printf("   %s",p->info.DATE);
		  printf("   %d",p->info.stime);
		  printf("   %d",p->info.atime);
		  printf("   %s",p->info.TYPE);
		  printf("   %d RMB",p->info.value);
          printf("\n");      
        p=p->next;   
    }   
          printf("\n\n");      
     
}   
//通过航班号实现二分查找法查找    
void HBH(struct flight F[])       
{   
    int low=0,high=N,mid;   
    char Num[10];   
    printf("Please enter your query number?\n");   
    scanf("%s",Num);   
    info();   
    while(low<=high)   
    {   
        mid=(low+high)/2;   
        if(strcmp(Num,F[mid].number)==0) 
		{
			Print(F,mid);
			break;
		}   
        else if(strcmp(Num,F[mid].number)<0) 
			high=mid-1;
		else low=mid+1;   
    }   
}   
//通过起飞时间查询    
void QFTime(struct flight F[])         
{   
    int T,i;   
    printf("Please enter your query stime(2000:20:00):\n");   
    scanf("%d",&T);   
    info();                                        
    for(i=0;i<N;i++)   
    {   
        if(T==F[i].stime) Print(F,i);   
   }    
}   
//通过到达时间查询 
void DDTime(struct flight F[])         
{   
    int T,i;   
    printf("Please enter your query atime(2000:20:00):\n");   
    scanf("%d",&T);   
    info();                                        
    for(i=0;i<N;i++)   
    {   
        if(T==F[i].atime) Print(F,i);   
    }    
}   
//通过起点查询    
void QDaddress(struct flight F[])       
{   
    char str[10]; 
    int i;   
    printf("Please enter your query staddr:\n");  
    scanf("%s",str);    
    info();   
    for(i=0;i<N;i++)   
    {   
        if(strcmp(str,F[i].staddress)==0)  Print(F,i);   
    }   
}   
//通过终点查询    
void ZDaddress(struct flight F[])       
{   
    char str[10]; 
    int i;   
    printf("Please enter your query arraddr:\n");   
    scanf("%s",str);    
    info();   
    for(i=0;i<N;i++)   
    {   
        if(strcmp(str,F[i].arraddress)==0)  
        	Print(F,i);   
    }   
}   
void shuru()   
{   
    strcpy(ys[0].key,"      ");   
    strcpy(ys[0].info.number,"      ");   
    strcpy(ys[0].info.staddress,"    ");   
    strcpy(ys[0].info.arraddress,"    ");   
    strcpy(ys[0].info.DATE,"    ");   
    ys[0].info.stime=0;   
    ys[0].info.atime=0;   
    strcpy(ys[0].info.TYPE,"  ");   
    ys[0].info.value=0;   
    ys[0].next=NULL;   
    int j;   
    char flag;   
    for(j=1;;j++)   
    {   
        printf("      No  staddr  arraddr  DATE  stime  atime  TYPE  value\n");    
        printf("input:");   
        scanf("%s %s %s %s %d %d %s %d",ys[j].info.number,ys[j].info.staddress,ys[j].info.arraddress,ys[j].info.DATE,&ys[j].info.stime,&ys[j].info.atime,ys[j].info.TYPE,&ys[j].info.value);   
        strcpy(ys[j].key,ys[j].info.number);   
        ys[j].next=NULL;  
//	   flag1++;	
        printf("prompt: Continue to enter?y/n:\n");   
        getchar();   
        scanf("%c",&flag);   
        if(flag=='n'||flag=='N')break;   
    }    
} 
