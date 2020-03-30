/*************************************************************************
	> File Name: server.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2018年07月16日 星期一 16时17分18秒
 ************************************************************************/
#include "comm.h"
int main()
{
	int shmid=GetShmid(4097);
    if(shmid>0)
    {   
        int i=0;
        char *addr=shmat(shmid,NULL,0);
        while(i<20)
        {   
            printf("%s\n",addr);
            sleep(1);
            i++;
        }   
        if(shmdt(addr)==-1)
        {   
			perror("shmdt");
			return -1;
	     }   
	}   
	else
	{   
		perror("GetShmid");
	    return -2;
	}
   return 0;
}

