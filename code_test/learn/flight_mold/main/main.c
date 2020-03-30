#include<stdio.h>
#include "../include/flight.h"
//主 函 数    
int main()   
{   
    int choose;
    int i;   
    shuru();//数据输入    
    jsList p=ys;   
    for(i=0;i<N;i++)   
    ys[i].next=&ys[i+1];   
    ys[10].next=NULL;   
    jsSort(&p, N1, N2);  //基数排序    
    ALL(ys);//输出航班表     
    copy(Flight,ys); //另存储排序后的航班信息    
    //相关查找处理    
    for(;;)   
    {   
        printf("******************************************\n");   
        printf("*    Flight information query system     *\n");   
        printf("******************************************\n");   
        printf("*        1.number       2.staddr         *\n");   
        printf("*        3.arraddr      4.stime          *\n");   
        printf("*        5.atime        0.exit           *\n");   
        printf("******************************************\n");   
        printf("Please choice(0-5):");   
        scanf("%d",&choose);   
        getchar();   
        if(choose!=0)   
        {   
            switch(choose)   
            {   
            case 1:{   
                        HBH(Flight);   
                   }break;   
            case 2: {   
                        QDaddress(Flight);   
                    }break;   
            case 3:{                       
                        ZDaddress(Flight);   
                   }break;   
            case 4:{                       
                        QFTime(Flight);   
                   }break;   
            case 5:{                       
                        DDTime(Flight);   
                   }break;   
			default:
				   printf("error! Please input\n");
				   break;   
            }   
        }   
        else    
        {   
            printf("Beybey!\n");   
            break;   
        }   
    } 
    return 0;  
}  
