/*************************************************************************
	> File Name: test.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2020年04月02日 星期四 15时38分05秒
 ************************************************************************/

#include<stdio.h>


void main()
{	
	unsigned char  val = 0x02;
	
	printf("  0x%X \n",val);
	printf("& 0x%X \n",val&0x7f);

	printf("| 0x%X \n",val|0x7f);

	printf("^ 0x%X \n",val^0x7f);

	printf("~ 0x%X \n\n",~0x7f);

}