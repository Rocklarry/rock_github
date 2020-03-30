/*************************************************************************
	> File Name: uart_test.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2017年07月18日 星期二 10时13分11秒
 ************************************************************************/

#include<stdio.h>
#include <string.h>
#include "DataType.h"
#include "mycom.h"



int main()
{
	
	printf("UART test start \n");
	ST_INT32 rxnum = 0;

	InitCom();
while(1)
	{
	memset( Uart[EXBOARDCOM].revbuf, 0, sizeof( Uart[EXBOARDCOM].revbuf ) );// Clear receive buffer

	rxnum= ReadCom( Screenfd, Uart[SCREENCOM].revbuf,
		sizeof( Uart[SCREENCOM].revbuf ), Com_Delay * 1000 );


	//write( Screenfd, Uart[SCREENCOM].sendbuf, 8 );

	printf("teset %d  but=%s\n",rxnum,Uart[SCREENCOM].revbuf);
	}
}