/*************************************************************************
	> File Name: comm.h
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2018年07月16日 星期一 16时07分35秒
 ************************************************************************/

#ifndef __COMM_H__
#define __COMM_H__
#include <stdio.h>
#include <error.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define PATHNAME "."
#define PROJ_ID 066

int CreatShmid(int size);
int GetShmid(int size);
int Destory(int shmid);

#endif
