#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <pthread.h>
#include <sys/wait.h>


char stop = 0;
char buf1[40960],buf2[40960],buf3[40960];

pthread_t ThreadId0;
pthread_t ThreadId1;
pthread_t ThreadId2;

int rx_vedio,tx_data,rx_data;
int usb_port0,usb_port1,usb_port2,usb_port3;

void Stop(int signo)
{
	
	close(rx_vedio);
	close(tx_data);
	close(rx_data);
	_exit(0);
	
}

/* Thread_Rx_Vedio用于演示视频接收操作流程。
 * 通过/dev/artosyn_port1设备节点来获取视频流信息，并保存到本地文件中。
*/

void *Thread_Rx_Vedio(void *p)
{

	int r_cnt,w_cnt;
	int ret;

	while(!stop)
	{
		
		r_cnt = read(usb_port1,buf1,20480);
		if(r_cnt < 0)	
		{
			close(usb_port1);
			do{
				usb_port1 = open("/dev/artosyn_port1", O_RDWR, S_IRUSR | S_IWUSR);
				usleep(100 * 1000);
			}while(usb_port1 <= 0);
			continue;
		}
		else if(r_cnt > 0)
		{
			w_cnt = 0;
			do{
					
				ret = write(rx_vedio,&buf1[w_cnt],r_cnt - w_cnt);
				w_cnt += ret;
					
			}while(w_cnt < r_cnt);
			usleep(20000);
		}
		usleep(10000);
		
 	}

	pthread_exit(NULL);
}

/* thread1用于演示数传信息发送操作流程。
 * 通过读本地数传文件，向/dev/artosyn_port3写数据，来执行数传的发送操作。
 * 数传文件中存放的是演示数据，并不是真正的数传信息，这个操作，只是向用户演示如何通过数传口发送数据功能。
*/

void *Thread_Tx_Data(void *p)
{
	int r_cnt,w_cnt;
	int ret;

	while(!stop)
	{
		r_cnt = read(tx_data,buf2,40960);
		if(r_cnt > 0)
		{
			w_cnt = 0;
			do{
				//每次写的时候，大小最好能4字节对齐
				ret = write(usb_port3,&buf2[w_cnt],r_cnt - w_cnt);
				if(ret < 0)	
				{
					//关闭设备
					close(usb_port3);
					//监测usb端口
					do{
						usb_port3 = open("/dev/artosyn_port3", O_RDWR, S_IRUSR | S_IWUSR);
						usleep(100 * 1000);
					}while(usb_port3 <= 0);
					continue;
				}
				else if(ret > 0)
					w_cnt += ret;
				else
					usleep(100);
				
			}while(w_cnt < r_cnt);
		}
		else if(r_cnt == 0)
 		{
 			close(tx_data);
			tx_data = open("send_data", O_RDWR, S_IRUSR | S_IWUSR);
 		}
 	}

	pthread_exit(NULL);
}

/* thread2用于演示数传信息接收操作流程。
 * 通过读/dev/artosyn_port3，来获取数传信息并保存到文件中。
*/

void *Thread_Rx_Data(void *p)
{
	int r_cnt,w_cnt;
	int ret;

	while(!stop)
	{
		r_cnt = read(usb_port3,buf3,40960);
		if(r_cnt > 0)
		{
			w_cnt = 0;
			do{
				//每次写的时候，大小最好能4字节对齐
				ret = write(rx_data,&buf3[w_cnt],r_cnt - w_cnt);
				if(ret < 0)	
				{
					//关闭设备
					close(usb_port3);
					//监测usb端口
					do{
						usb_port3 = open("/dev/artosyn_port3", O_RDWR, S_IRUSR | S_IWUSR);
						usleep(100 * 1000);
					}while(usb_port3 <= 0);
					continue;
				}
				else if(ret > 0)
					w_cnt += ret;
				else
					usleep(100);		
			}while(w_cnt < r_cnt);
		}
 	}

	pthread_exit(NULL);
}


int main()
{
	signal(SIGINT,Stop);
	
	usb_port1 = open("/dev/artosyn_port1", O_RDWR, S_IRUSR | S_IWUSR);
	usb_port3 = open("/dev/artosyn_port3", O_RDWR, S_IRUSR | S_IWUSR);
	
	rx_vedio = open("send_vedio", O_RDWR, S_IRUSR | S_IWUSR);
	tx_data = open("send_data", O_RDWR, S_IRUSR | S_IWUSR);
	rx_data = open("rec_data", O_RDWR, S_IRUSR | S_IWUSR);
	
	if(usb_port1 < 0 || usb_port3 < 0 || rx_vedio < 0 || tx_data < 0 || rx_data < 0)
	{
		printf("open err\n");
		return -1;			
	}
		
	if(pthread_create(&ThreadId0, NULL, Thread_Rx_Vedio, NULL) != 0)
	{
		close(rx_vedio);
		close(tx_data);
		close(rx_data);
		printf("thread0 creat err\n");
		return 0;
	}
	
	if(pthread_create(&ThreadId1, NULL, Thread_Tx_Data, NULL) != 0)
	{
		stop = 1;
		usleep(100 * 1000);
		pthread_join(ThreadId0,NULL);
		close(rx_vedio);
		close(tx_data);
		close(rx_data);
		printf("thread1 creat err\n");
		return 0;
	}
	
	if(pthread_create(&ThreadId2, NULL, Thread_Rx_Data, NULL) != 0)
	{
		stop = 1;
		usleep(100 * 1000);
		
		pthread_join(ThreadId1,NULL);
		pthread_join(ThreadId2,NULL);
		close(rx_vedio);
		close(tx_data);
		close(rx_data);
		printf("thread2 creat err\n");
		return 0;
	}
	
	while(1)
		usleep(1000000);
		
	pthread_join(ThreadId0,NULL);
	pthread_join(ThreadId1,NULL);
	pthread_join(ThreadId2,NULL);
	
	close(rx_vedio);
	close(tx_data);
	close(rx_data);
	
  return 0;
}

