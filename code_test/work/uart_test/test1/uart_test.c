/*************************************************************************
	> File Name: teset.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2017年07月18日 星期二 14时43分06秒
 ************************************************************************/



#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include<string.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#define BAUDRATE        B115200
#define UART_DEVICE     "/dev/ttyHSL2"

#define FALSE  -1
#define TRUE   0

/*
  dev/ttyGS0   
  dev/ttyHS0   
  dev/ttyHSL0  
  dev/ttyHSL1  
  dev/ttyHSL2

*/
////////////////////////////////////////////////////////////////////////////////
/**
*@brief  设置串口通信速率
*@param  fd     类型 int  打开串口的文件句柄
*@param  speed  类型 int  串口速度
*@return  void
*/
int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300,
          		   B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = {115200, 38400, 19200, 9600, 4800, 2400, 1200,  300, 
		  		  115200, 38400, 19200, 9600, 4800, 2400, 1200,  300, };
// 串口的文件句柄
int    fd ;

static unsigned char  read_buf[128];
static unsigned char  cont_buf[128];
static float    ir_distance;  


void set_speed(int fd, int speed){
  int   i; 
  int   status; 
  struct termios   Opt;
  tcgetattr(fd, &Opt); 
  for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) { 
    if  (speed == name_arr[i]) {     
      tcflush(fd, TCIOFLUSH);     
      cfsetispeed(&Opt, speed_arr[i]);  
      cfsetospeed(&Opt, speed_arr[i]);   
      status = tcsetattr(fd, TCSANOW, &Opt);  
      if  (status != 0) {        
        perror("tcsetattr fd1");  
        return;     
      }    
      tcflush(fd,TCIOFLUSH);   
    }  
  }
}
////////////////////////////////////////////////////////////////////////////////
/**
*@brief   设置串口数据位，停止位和效验位
*@param  fd     类型  int  打开的串口文件句柄
*@param  databits 类型  int 数据位   取值 为 7 或者8
*@param  stopbits 类型  int 停止位   取值为 1 或者2
*@param  parity  类型  int  效验类型 取值为N,E,O,,S
*/
int set_Parity(int fd,int databits,int stopbits,int parity)
{ 
	struct termios options; 
	if  ( tcgetattr( fd,&options)  !=  0) { 
		perror("SetupSerial 1");     
		return(FALSE);  
	}
	options.c_cflag &= ~CSIZE; 
	switch (databits) /*设置数据位数*/
	{   
	case 7:		
		options.c_cflag |= CS7; 
		break;
	case 8:     
		options.c_cflag |= CS8;
		break;   
	default:    
		fprintf(stderr,"Unsupported data size\n"); return (FALSE);  
	}
	switch (parity) 
	{   
		case 'n':
		case 'N':    
			options.c_cflag &= ~PARENB;   /* Clear parity enable */
			options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
			break;  
		case 'o':   
		case 'O':     
			options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/  
			options.c_iflag |= INPCK;             /* Disnable parity checking */ 
			break;  
		case 'e':  
		case 'E':   
			options.c_cflag |= PARENB;     /* Enable parity */    
			options.c_cflag &= ~PARODD;   /* 转换为偶效验*/     
			options.c_iflag |= INPCK;       /* Disnable parity checking */
			break;
		case 'S': 
		case 's':  /*as no parity*/   
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;break;  
		default:   
			fprintf(stderr,"Unsupported parity\n");    
			return (FALSE);  
		}  
	/* 设置停止位*/  
	switch (stopbits)
	{   
		case 1:    
			options.c_cflag &= ~CSTOPB;  
			break;  
		case 2:    
			options.c_cflag |= CSTOPB;  
		   break;
		default:    
			 fprintf(stderr,"Unsupported stop bits\n");  
			 return (FALSE); 
	} 
	/* Set input parity option */ 
	if (parity != 'n')   
		options.c_iflag |= INPCK; 
	tcflush(fd,TCIFLUSH);
	options.c_cc[VTIME] = 150; /* 设置超时15 seconds*/   
	options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
	if (tcsetattr(fd,TCSANOW,&options) != 0)   
	{ 
		perror("SetupSerial 3");   
		return (FALSE);  
	} 
	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
	options.c_oflag  &= ~OPOST;   /*Output*/
	return (TRUE);  
}


void  init_uart()
{
	 printf("Start  init UART ...\n");
    fd = open(UART_DEVICE, O_RDWR);

    if (fd < 0) {
        perror(UART_DEVICE);
        exit(1);
    }

    printf("Open...\n");
    set_speed(fd,115200);
	if (set_Parity(fd,8,1,'N') == FALSE)  {
		printf("Set Parity Error\n");
		exit (0);
	}

}


void debug_uart()
{

printf("%s ****\n",__func__);
system("mount  -t debugfs none /sys/kernel/debug");
//system("echo 1 >  /sys/kernel/debug/msm_serial_hs/loopback.0");

}


////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{

    int   res;
    char  buf[256];
	 struct timeval tv;

	//debug_uart();
	init_uart();

    printf("Reading...\n");
    while(1) {
		buf[res]=0;
		tcflush(fd,TCIOFLUSH);
        res = read(fd, buf, 255);

        if(res==1)
			{
			// printf("res = %d \n",res);
             continue;
			}
       // buf[res]=0;
	   // printf("microsecond:%ld\n",tv.tv_sec*1000000 + tv.tv_usec);  //微秒
	    gettimeofday(&tv,NULL);
        printf(" microsecond:%ld     res = %d buf = %s",tv.tv_sec*1000000 + tv.tv_usec,res, buf);
		
		//read_buf[128]=0;
		//cont_buf[128]=0;
		//memcpy(read_buf, buf+10, 4);
		//memcpy(cont_buf, buf+0, 1);
		//ir_distance = atoi((const char*)read_buf)*0.01;
		//printf("cont_buf = %s, ir_distance = %f \n",cont_buf,ir_distance);
        
        /*if (buf[0] == 0x0d)
        	printf("\n");
        
        if (buf[0] == '@') break;
		*/
    }

    printf("Close...\n");
    close(fd);

    return 0;
}
