/*************************************************************************
	> File Name: comm.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2018年07月16日 星期一 16时12分45秒
 ************************************************************************/

#include "comm.h"

static int commShmid(int size, int flag)
{
	key_t key=ftok(PATHNAME,PROJ_ID);
	if(key>0)
	{
		return shmget(key,size,flag);
	}
	else
	{
		perror("ftok");
		return -1;
	}
}


int CreatShmid(int size)
{
	return commShmid(size,IPC_CREAT|IPC_EXCL|0666);
}


int GetShmid(int size)
{
	    return commShmid(size,IPC_CREAT);
}
int Destory(int shmid)
{
	    return shmctl(shmid,0,IPC_RMID);
}

