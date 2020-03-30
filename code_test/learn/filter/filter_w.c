/*************************************************************************
	> File Name: filter_w.c
	> Author: Rock
	> Mail: ruidongren@163.com 
	> Created Time: 2017年03月23日 星期四 16时37分05秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>

#define FILTERNUM 6

int mx_scaled[FILTERNUM];
int my_scaled[FILTERNUM];
int mz_scaled[FILTERNUM];

//static int mag_data_x;
//static int mag_data_y;
//static int mag_data_z;




int i;
int y;
static int x;
//void filter(int mag_data_x,int mag_data_y,int mag_data_z)
void filter(int mag_data_x,int mag_data_y,int mag_data_z)
{


int sumX =0;
int sumY =0;
int sumZ =0;


for(i=0;i<FILTERNUM-1;i++)
	{
	mx_scaled[i] = mx_scaled[i+1];
	my_scaled[i] = my_scaled[i+1];
	mz_scaled[i] = mz_scaled[i+1];

	printf("mx_scaled=%d     my_scaled=%d     mz_scaled=%d   \n",mx_scaled[i],my_scaled[i],mz_scaled[i]);

  }
   mx_scaled[FILTERNUM] = mag_data_x;
   my_scaled[FILTERNUM] = mag_data_y;
   mz_scaled[FILTERNUM] = mag_data_z;


printf("mx_scaled=%d     my_scaled=%d     mz_scaled=%d  mag_data_x=%d mag_data_y=%d mag_data_z=%d   \n\n\n\n\n",mx_scaled[FILTERNUM-1],my_scaled[FILTERNUM-1],mz_scaled[FILTERNUM-1],mag_data_x,mag_data_y,mag_data_z);
	for( i=0;i<FILTERNUM;i++)
	{
	 sumX += mx_scaled[i];
	 sumY += my_scaled[i];
	 sumZ += mz_scaled[i];

printf("sumX=%d sumY=%d sumZ=%d mx_scaled=%d     my_scaled=%d     mz_scaled=%d\n",sumX,sumY,sumZ,mx_scaled[i],my_scaled[i],mz_scaled[i]);
	}


  mag_data_x= sumX/FILTERNUM;
  mag_data_y= sumY/FILTERNUM;
  mag_data_z= sumZ/FILTERNUM;

   mx_scaled[FILTERNUM-1] = mag_data_x;
   my_scaled[FILTERNUM-1] = mag_data_y;
   mz_scaled[FILTERNUM-1] = mag_data_z;


printf("mag_data_x=%d mag_data_y=%d mag_data_z=%d \n",mag_data_x,mag_data_y,mag_data_z);

}

// 用于随机产生一个300左右的当前值
long int Get_AD() {
	int m=50;
	int n = 60;
  return rand()%(n-m+1)+m;
}

#define FILTER_N 12
int filter_buf[FILTER_N + 1];
int Filter() {
  int i;
  int filter_sum = 0;
  filter_buf[FILTER_N] = Get_AD();
  for(i = 0; i < FILTER_N; i++) {
    filter_buf[i] = filter_buf[i + 1]; // 所有数据左移，低位仍掉
    filter_sum += filter_buf[i];

	printf(" filter_buf= %d",filter_buf[i]);
  }
  return (int)(filter_sum / FILTER_N);
}

int main(void)
{
 int i;


 printf("\n\n\n\n Filter =%d \n\n\n\n",Filter());
 //for(i=0;i<10;i++)
	 //filter(i,i,i);

	filter(2,4,6);
	filter(2,4,6);
	filter(2,4,6);
	filter(2,4,6);
	filter(2,4,6);
	filter(2,4,6);
	filter(2,4,6);
	filter(2,4,6);
	filter(2,4,6);
	filter(2,4,6);
	filter(2,4,6);
	filter(2,4,6);
	filter(2,4,6);
	filter(2,4,6);
	filter(2,4,6);
	filter(2,4,6);
	filter(2,4,6);
	filter(2,4,6);

	


 return 0;
}


