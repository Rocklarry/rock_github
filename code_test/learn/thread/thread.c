/*************************************************************************
	> File Name: thread.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2017年10月11日 星期三 11时47分15秒
 ************************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#define MAX 10

pthread_t thread[2]; //创建线程函数返回类型
pthread_mutex_t mut; //互斥锁类型
int number=0, i;

void *thread1() //线程函数
{
  printf ("thread1 : I'm thread 1\n");
    
  for (i = 0; i < MAX; i++)
  {   
  printf("thread1 : number = %d\n",number);   
  pthread_mutex_lock(&mut); //加锁，用于对共享变量操作
  number++;
  pthread_mutex_unlock(&mut); //解锁
  usleep(300*1000);
  }   
    

  printf("thread1 :主函数在等我完成任务吗？\n");
  pthread_exit(NULL);
}

void *thread2()
{
  printf("thread2 : I'm thread 2\n");
    
  for (i = 0; i < MAX; i++)
  {   
  printf("thread2 : number = %d\n",number);
  pthread_mutex_lock(&mut);
  number++;
  pthread_mutex_unlock(&mut);
  usleep(800*1000);
  }   
    

  printf("thread2 :主函数在等我完成任务吗？\n");
  pthread_exit(NULL);
}

void thread_create(void)
{
  /*创建线程*/
  pthread_create(&thread[0], NULL, thread1, NULL);
  printf("线程1被创建\n");
  pthread_create(&thread[1], NULL, thread2, NULL);
  printf("线程2被创建\n");
}

void thread_wait(void)
{
  /*等待线程结束*/
  pthread_join(thread[0],NULL);
  printf("线程1已经结束\n");
  pthread_join(thread[1],NULL);
  printf("线程2已经结束\n");
}

int main()
{
  /*用默认属性初始化互斥锁*/
  pthread_mutex_init(&mut,NULL);  
    
  printf("我是主函数哦，我正在创建线程，呵呵\n");
  thread_create();
  printf("我是主函数哦，我正在等待线程完成任务阿，呵呵\n");
  thread_wait();
    
  return 0;
}