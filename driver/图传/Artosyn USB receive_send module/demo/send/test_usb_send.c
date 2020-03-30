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

char image[40960];
int fd_usb;
int fd_file;
pthread_t thread_id;

void stop(int signo)
{
	close(fd_usb);
	close(fd_file);

	_exit(0);
}


void *thread0(void *p)
{

	int r_cnt,w_cnt;
	int ret;

	fd_usb = open("/dev/artosyn_port1", O_RDWR, S_IRUSR | S_IWUSR);

	printf("open   %d  \n",fd_usb);
	fd_file = open("usbtest.264", O_RDWR, S_IRUSR | S_IWUSR);
	
	while(1)
	{
		r_cnt = read(fd_file,image,40960);
		if(r_cnt > 0)
		{	
			w_cnt = 0;
			do{
				
				
				//每次写的时候，大小最好能4字节对齐
				ret = write(fd_usb,&image[w_cnt],r_cnt - w_cnt);

				printf("write   r_cnt=%d \n",r_cnt);
				if(ret < 0)	
				{
					//关闭设备
					close(fd_usb);
					
					//监测usb端口
					do{
						
						fd_usb = open("/dev/artosyn_port1", O_RDWR, S_IRUSR | S_IWUSR);
						usleep(100 * 1000);
					}while(fd_usb <= 0);
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
 			close(fd_file);
			fd_file = open("filetosend", O_RDWR, S_IRUSR | S_IWUSR);
 		}
 	}

	close(fd_usb);
	close(fd_file);

	pthread_exit(NULL);
}

int main()
{
	signal(SIGINT,stop);

	printf("send start \n");
	
	if(pthread_create(&thread_id, NULL, thread0, NULL) != 0)
	{
		close(fd_usb);
		close(fd_file);
		printf("thread creat err\n");
		return 0;
	}
	
	while(1)
		usleep(1000000);
		
	pthread_join(thread_id,NULL);

	printf("pthread_join start \n");
	
  return 0;
}

