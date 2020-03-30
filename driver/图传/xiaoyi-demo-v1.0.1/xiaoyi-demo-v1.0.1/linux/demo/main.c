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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const unsigned int USBFPV_PORT = 16884;
#define CAMERA_SUPER_LOCATION "/camera_super/"
const char* log_file_name="/camera_super/usb_fpv.log";

char stop = 0;
char buf1[40960],buf2[40960],buf3[40960];
char data[2] = {'\0'};

pthread_t ThreadId0;
pthread_t ThreadId1;
pthread_t ThreadId2;
int usb_fpv_fd = -1;
struct sockaddr_in usb_fpv_udpAddress;

int tx_vedio,tx_data,rx_data;
int usb_port0,usb_port1,usb_port2,usb_port3;

void Stop(int signo)
{
	close(tx_vedio);
	close(tx_data);
	close(rx_data);
	_exit(0);
}

/* Thread_Tx_Vedio������ʾ��Ƶ���Ͳ������̡�
 * ͨ����������Ƶ�ļ�����/dev/artosyn_port1д���ݣ���ִ����Ƶ�ķ��Ͳ�����
*/

void *Thread_Tx_Vedio(void *p)
{

	int r_cnt,w_cnt;
	int ret;
	long g_curr_offset = 0; 

	while(!stop)
	{
		if (lseek(tx_vedio, g_curr_offset, SEEK_SET) == -1)
        {
            printf("lseek error\n");
            break;
        }
		memset(buf1, 0x0, 40960); 

		r_cnt = read(tx_vedio,buf1,40960);
		if(r_cnt > 0)
		{
			w_cnt = 0;
			do{
				//ÿ��д��ʱ�򣬴�С�����4�ֽڶ���
				ret = write(usb_port1,&buf1[w_cnt],r_cnt - w_cnt);
				if(ret < 0)	
				{
					//�ر��豸
					close(usb_port1);
					//���usb�˿�
					do{
						usb_port1 = open("/dev/artosyn_port1", O_RDWR, S_IRUSR | S_IWUSR);
						usleep(100 * 1000);
					}while(usb_port1 <= 0);
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
 			close(tx_vedio);
			tx_vedio = open("fpv.h264", O_RDWR, S_IRUSR | S_IWUSR);
 		}
		g_curr_offset += r_cnt;
 	}

	pthread_exit(NULL);
}

/* thread1������ʾ������Ϣ���Ͳ������̡�
 * ͨ�������������ļ�����/dev/artosyn_port3д���ݣ���ִ�������ķ��Ͳ�����
 * �����ļ��д�ŵ�����ʾ���ݣ�������������������Ϣ�����������ֻ�����û���ʾ���ͨ�������ڷ������ݹ��ܡ�
*/

void *Thread_Tx_Data(void *p)
{
	int r_cnt,w_cnt;
	int ret;

	while(!stop)
	{
		r_cnt = read(tx_data,buf2,2048);
		if(r_cnt > 0)
		{
			w_cnt = 0;
			do{
				//ÿ��д��ʱ�򣬴�С�����4�ֽڶ���
				ret = write(usb_port3,&buf2[w_cnt],r_cnt - w_cnt);
				if(ret < 0)	
				{
					//�ر��豸
					close(usb_port3);
					//���usb�˿�
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
 		usleep(10000);
 	}

	pthread_exit(NULL);
}

/* thread2������ʾ������Ϣ���ղ������̡�
 * ͨ����/dev/artosyn_port3������ȡ������Ϣ�����浽�ļ��С�
*/

void *Thread_Rx_Data(void *p)
{
	int r_cnt,w_cnt;
	int ret;
	int i;

	while(!stop)
	{
		r_cnt = read(usb_port3,buf3,40960);

		//printf(" usb read :%s r_cnt=%d \n",buf3,r_cnt);
		if(r_cnt > 0)
		{
			
			w_cnt = 0;
			do{
				//ÿ��д��ʱ�򣬴�С�����4�ֽڶ���
				ret = write(rx_data,&buf3[w_cnt],r_cnt - w_cnt);
				printf(" usb read %d \n",buf3[0]);
				memset(data,0,2);
                sprintf(data, "%d", buf3[0]);
				printf("usb_fpv_fd is %d, data is %s\n", usb_fpv_fd, data);
				int len =  sendto(usb_fpv_fd,data, strlen(data), 0, (struct sockaddr*)&usb_fpv_udpAddress, sizeof(usb_fpv_udpAddress)); 
				printf("send usbfpv write 1 len =%d\n", len);
				if(len < 0)
				{
				    usleep(20000);
					len =  sendto(usb_fpv_fd,data, strlen(data), 0, (struct sockaddr*)&usb_fpv_udpAddress, sizeof(usb_fpv_udpAddress)); 
				}
				
				if(ret < 0)	
				{
					//�ر��豸
					close(usb_port3);
					//���usb�˿�
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
	if(access(CAMERA_SUPER_LOCATION, 0)==0)
		printf("%s already created!\n",CAMERA_SUPER_LOCATION);
	else{
		int status = mkdir(CAMERA_SUPER_LOCATION, 0777);
		if(status==0)
			printf("create %s!\n",CAMERA_SUPER_LOCATION);
		else{
			printf("create %s failed, Setting to default path\n", CAMERA_SUPER_LOCATION);
		}
	}	
	/*tail -f /camera_super/camera_super.log*/
	FILE *fp;
	if ((fp = fopen(log_file_name, "w+")) != NULL)
	{
		fclose(fp);
	}

	freopen(log_file_name, "a", stdout); setbuf(stdout, NULL);
	freopen(log_file_name, "a", stderr); setbuf(stderr, NULL);
	
	int socket_cli;
	bzero(&usb_fpv_udpAddress, sizeof(usb_fpv_udpAddress));
	usb_fpv_udpAddress.sin_family = AF_INET;
	usb_fpv_udpAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	usb_fpv_udpAddress.sin_port = htons(USBFPV_PORT);
	usb_fpv_fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	usb_port1 = open("/dev/artosyn_port1", O_RDWR, S_IRUSR | S_IWUSR);
	usb_port3 = open("/dev/artosyn_port3", O_RDWR, S_IRUSR | S_IWUSR);
	
	tx_vedio = open("fpv.h264", O_RDWR, S_IRUSR | S_IWUSR);
	tx_data = open("send_data", O_RDWR, S_IRUSR | S_IWUSR);
	rx_data = open("rec_data", O_RDWR, S_IRUSR | S_IWUSR);
	
	if(usb_port1 < 0 || usb_port3 < 0 || tx_vedio < 0 || tx_data < 0 || rx_data < 0)
	{
		printf("open err\n");
		return -1;			
	}
		
	if(pthread_create(&ThreadId0, NULL, Thread_Tx_Vedio, NULL) != 0)
	{
		close(tx_vedio);
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
		close(tx_vedio);
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
		close(tx_vedio);
		close(tx_data);
		close(rx_data);
		printf("thread2 creat err\n");
		return 0;
	}
/*	int socket_cli;
	bzero(&usb_fpv_udpAddress, sizeof(usb_fpv_udpAddress));
	usb_fpv_udpAddress.sin_family = AF_INET;
	usb_fpv_udpAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	usb_fpv_udpAddress.sin_port = htons(USBFPV_PORT);
	usb_fpv_fd = socket(AF_INET, SOCK_DGRAM, 0);	*/
	
	while(1)
		usleep(1000000);
		
	pthread_join(ThreadId0,NULL);
	pthread_join(ThreadId1,NULL);
	pthread_join(ThreadId2,NULL);
	
	close(tx_vedio);
	close(tx_data);
	close(rx_data);
	
  return 0;
}

