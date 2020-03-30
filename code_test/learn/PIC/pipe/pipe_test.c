/*************************************************************************
	> File Name: pipe_test.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2018年07月16日 星期一 15时15分55秒
 ************************************************************************/

 /*

 每个进程各自有不同的用户地址空间,任何一个进程的全局变量在另一个进程中都看不到，
 所以进程之间要交换数据必须通过内核,在内核中开辟一块缓冲区,进程A把数据从用户空间拷到内核缓冲区,
 进程B再从内核缓冲区把数据读走,内核提供的这种机制称为进程间通信。

管道通信创建方式

int pipe (int fd[2]);	//返回 成功 0  失败 -1

fd参数返回两个文件描述符 fd[0]指向管道读端  fd[1]指向写端   fd[1]输出是fd[0]的输入
 */

#include <stdio.h>  
#include <unistd.h>  
#include <string.h>  
#include <errno.h>  

int main()  
{  
    int fd[2];  
    int ret = pipe(fd);  
    if (ret == -1)  
    {  
        perror("pipe error\n");  
        return 1;  
    }  
    pid_t id = fork();  
    if (id == 0)  
    {//child  
        int i = 0;  
        close(fd[0]);  
        char *child = "I am  child!";  
        while (i<5)  
        {  
            write(fd[1], child, strlen(child) + 1);  
            sleep(2);  
            i++;  
			printf("child i=%d\n",i);
        }  
    }  
    else if (id>0)  
    {//father  
        close(fd[1]);  
        char msg[100];  
        int j = 0;  
        while (j<5)  
        {  
            memset(msg,'\0',sizeof(msg));  
            ssize_t s = read(fd[0], msg, sizeof(msg));  
            if (s>0)  
            {  
                msg[s - 1] = '\0';  
            }  
            printf("%s\n", msg);  
            j++;  
			printf("father j=%d\n",j);
        }  
    }  
    else  
    {//error  
        perror("fork error\n");  
        return 2;  
    }  
    return  0;  
}

