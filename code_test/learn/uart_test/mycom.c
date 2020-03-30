#include <stdio.h>      /*标准输入输出定义*/
#include <stdlib.h>
#include <unistd.h>     /*Unix标准函数定义*/
#include <sys/types.h>  /**/
#include <sys/stat.h>   /**/
#include <fcntl.h>      /*文件控制定义*/
#include <termios.h>    /*PPSIX终端控制定义*/
#include <errno.h>      /*错误号定义*/
#include <getopt.h>
#include <string.h>
#include "mycom.h"
#include "DataType.h"


#define FALSE 1
#define TRUE 0

UARTMODE Uart[NUM];

ST_CHAR *ExBoardCom = "/dev/ttySAC1"; 
ST_CHAR *CardCom = "/dev/ttySAC4"; 
ST_CHAR *ScreenCom = "/dev/ttyHSL2"; 
ST_CHAR *GPRSCom = "/dev/ttySAC2";
ST_INT32 ExBoardfd,Cardfd,Screenfd,GPRSfd;


ST_INT32 speed_arr[] = { 
	B921600, B460800, B230400, B115200, B57600, B38400, B19200, 
	B9600, B4800, B2400, B1200, B300, B38400, B19200, B9600, 
	B4800, B2400, B1200, B300, 
};
ST_INT32 name_arr[] = {
	921600, 460800, 230400, 115200, 57600, 38400,  19200,  
	9600,  4800,  2400,  1200,  300, 38400,  19200,  9600, 
	4800, 2400, 1200,  300, 
};


/********************************************************************************* 
 函数名称： set_speed
 功能描述： 设置波特率
 输    入：	fd：串口文件句柄，speed：波特率
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void set_speed(ST_INT32 fd, ST_INT32 speed)
{
	printf("%s fd=%d  speed=%d \n",__func__,fd,speed);
	ST_INT32   i;
	ST_INT32   status;
	struct termios   Opt;
	tcgetattr(fd, &Opt);

	for ( i= 0;  i < sizeof(speed_arr) / sizeof(ST_INT32);  i++) {
		if  (speed == name_arr[i])	{
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
			status = tcsetattr(fd, TCSANOW, &Opt);
			if  (status != 0)
				perror("tcsetattr fd1");
				return;
		}
		tcflush(fd,TCIOFLUSH);
   }
}


/********************************************************************************* 
 函数名称： set_Parity
 功能描述： 设置奇偶校验、停止位和数据位
 输    入：	fd：打开的串口文件句柄，databits：数据位，stopbits：停止位，parity：奇偶校验
 输	   出： 无
 返 回 值： TRUE：设置成功，FALSE：设置失败
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_INT32 set_Parity(ST_INT32 fd,ST_INT32 databits,ST_INT32 stopbits,ST_INT32 parity)
{

	printf("%s fd=%d  databits=%d  stopbits=%d parity=%d\n",__func__,fd,databits,stopbits,parity);

	struct termios options;
	if  ( tcgetattr( fd,&options)  !=  0) {
		perror("SetupSerial 1");
		return(FALSE);
	}
	options.c_cflag &= ~CSIZE ;
	switch (databits) /*设置数据位数*/ {
	case 7:
		options.c_cflag |= CS7;
	break;
	case 8:
		options.c_cflag |= CS8;
	break;
	default:
		fprintf(stderr,"Unsupported data size\n");
		return (FALSE);
	}
	
	switch (parity) {
	case 'n':
	case 'N':
		options.c_cflag &= ~PARENB;   /* Clear parity enable */
		options.c_iflag &= ~INPCK;     /* Enable parity checking */
	break;
	case 'o':
	case 'O':
		options.c_cflag |= (PARODD | PARENB);  /* 设置为奇效验*/
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
		options.c_cflag &= ~CSTOPB;
	break;
	default:
		fprintf(stderr,"Unsupported parity\n");
		return (FALSE);
	}
 	/* 设置停止位*/  
  	switch (stopbits) {
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

  	if (parity != 'n')
    	options.c_iflag |= INPCK;
  	options.c_cc[VTIME] = 150; // 15 seconds
    options.c_cc[VMIN] = 0;
	options.c_iflag &= ~(ICRNL | IXON);
	options.c_lflag &= ~(ECHO | ICANON | ISIG);
	
	options.c_iflag &= ~ (IXON | IXOFF | IXANY);

	options.c_iflag &= ~ (INLCR | ICRNL | IGNCR);
	options.c_oflag &= ~(ONLCR | OCRNL);


  	tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
  	if (tcsetattr(fd,TCSANOW,&options) != 0) {
    	perror("SetupSerial 3");
  		return (FALSE);
 	}
	return (TRUE);
}


/********************************************************************************* 
 函数名称： OpenDev
 功能描述： 打开串口
 输    入：	Dev：串口文件路径
 输	   出： 无
 返 回 值：-1：打开失败，否则返回串口文件句柄
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_INT32 OpenDev(ST_CHAR *Dev)
{
	ST_INT32 fd = open( Dev, O_RDWR ); 
 	if (-1 == fd) 
	{ /*设置数据位数*/
   		perror("Can't Open Serial Port");
   		return -1;
	} 
	else
		return fd;
}


/********************************************************************************* 
 函数名称： OpenCom
 功能描述： 打开串口
 输    入：	device：串口文件路径
 输	   出： 无
 返 回 值：	无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void OpenCom(ST_CHAR *device)
{ 

	Screenfd = OpenDev(device);
		if (Screenfd > 0)
		{
			set_speed(Screenfd,115200);
		}
		else
		{
			fprintf(stderr, "Error opening %s: %s\n", device, strerror(errno));
			exit(1);
		}
		if (set_Parity(Screenfd,8,1,'N')== FALSE)
		{
			fprintf(stderr, "Set Parity Error\n");
			close(Screenfd);
			exit(1);
		}
	/*
	if(!strcmp(device,ExBoardCom))
	{	
		ExBoardfd = OpenDev(device);
		if (ExBoardfd > 0)
		{
			set_speed(ExBoardfd,2400);
		}
		else
		{
			fprintf(stderr, "Error opening %s: %s\n", device, strerror(errno));
			exit(1);
		}
		if (set_Parity(ExBoardfd,8,1,'E')== FALSE)
		{
			fprintf(stderr, "Set Parity Error\n");
			close(ExBoardfd);
			exit(1);
		}

	}
	if(!strcmp(device,CardCom))
	{	
		Cardfd = OpenDev(device);
		if (Cardfd > 0)
		{
			set_speed(Cardfd,115200);
		}
		else
		{
			fprintf(stderr, "Error opening %s: %s\n", device, strerror(errno));
			exit(1);
		}
		if (set_Parity(Cardfd,8,1,'N')== FALSE)
		{
			fprintf(stderr, "Set Parity Error\n");
			close(Cardfd);
			exit(1);
		}

	}
	if(!strcmp(device,ScreenCom))
	{	
		Screenfd = OpenDev(device);
		if (Screenfd > 0)
		{
			set_speed(Screenfd,115200);
		}
		else
		{
			fprintf(stderr, "Error opening %s: %s\n", device, strerror(errno));
			exit(1);
		}
		if (set_Parity(Screenfd,8,1,'N')== FALSE)
		{
			fprintf(stderr, "Set Parity Error\n");
			close(Screenfd);
			exit(1);
		}

	}
	
	if(!strcmp(device,GPRSCom))
	{
		GPRSfd = OpenDev(device);
		if (GPRSfd>0)
		{
			set_speed(GPRSfd,9600);
		}
		else
		{
			fprintf(stderr, "Error opening %s: %s\n", device, strerror(errno));
			exit(1);
		}
		if (set_Parity(GPRSfd,8,1,'N')== FALSE)
		{
			fprintf(stderr, "Set Parity Error\n");
			close(GPRSfd);
			exit(1);
		}

	}*/
}


/********************************************************************************* 
 函数名称： InitCom
 功能描述： 初始化串口
 输    入：	无
 输	   出： 无
 返 回 值：	无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void InitCom(void)
{
	//OpenCom(ExBoardCom);
	//OpenCom(CardCom);
	OpenCom(ScreenCom);
	//OpenCom(GPRSCom);
	printf("%s dev = %s \n",__func__,ScreenCom);
}


ssize_t tread(ST_INT32 fd, ST_UCHAR *buf, size_t nbytes, ST_UINT32 timout)
{
	int nfds;
	fd_set readfds;
	struct timeval  tv;
	
	tv.tv_sec = 0;
	tv.tv_usec = timout;
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
	nfds = select(fd+1, &readfds, NULL, NULL, &tv);
	if (nfds <= 0)
	{
	if (nfds == 0)
	     errno = ETIME;
	return(-1);
	}
	return(read(fd, buf, nbytes));	
}

ssize_t ReadCom(ST_INT32 fd, ST_UCHAR *buf, size_t nbytes, ST_UINT32 timout)
{
	size_t      nleft;
	ssize_t     nread;
	
	nleft = nbytes;

	//printf("%s fd=%d    timout=%d \n",__func__,fd,timout);
	while (nleft > 0) 
	{
		if ((nread = tread(fd, buf, nleft, timout)) < 0) 
		{
			if (nleft == nbytes)
			return(-1); /* error, return -1 */
			else
			break;      /* error, return amount read so far */
		}
		else if (nread == 0) 
		{
			break;          /* EOF */
		}
		nleft -= nread;
		buf += nread;
	}
	return(nbytes - nleft);      /* return >= 0 */
}
