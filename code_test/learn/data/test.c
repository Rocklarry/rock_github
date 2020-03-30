/*************************************************************************
	> File Name: test.c
	> Author: Rock
	> Mail: ruidongren@163.com 
	> Created Time: 2017年02月21日 星期二 20时33分17秒
 ************************************************************************/

#include <stdio.h>
#include <stdio.h>

void test1()
{
	int adev_bdaddr[4];
	int i;

	for(i=0;i<=3;i++)
	{
	adev_bdaddr[i]=i;
	printf("test i[%d]=%d \n",i,i);
	}
}

int main()
{
test1();

int a=0x12345678;
a=((a&0xff00ff00) >>8) | ((a&0xff00ff)<<8);

 printf("\n\n     a = %d \n",a);
a=((a&0xffff0000)>>16) | ((a&0xffff)<<16);

 printf("\n\n     a = %d \n",a);

return 0;
}
