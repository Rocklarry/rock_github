/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
//#define SPI_DEV_PATH   "/dev/spi-1"
//#define SPI_INT_GPIO   65  // GPIO device for MPU data ready interrupt

#include <mpu9x50.h>
//#include "low_pass_filter.h"

//second_order_butterworth_lpf_t lpf;
#include <string.h>
#include <stdio.h>    
#include <sys/time.h> 


float b[] = {0.6082,
				1.1479,
				2.5467,
				2.4473,
				2.5467,
				1.1479,
				0.6082}; 
float a[] = {
			1.,
			1.5806,
			2.8675,
			2.3502,
			2.0725,
			0.8118,
			0.3698};
			
float g_axf[20]={0};
float g_ayf[20]={0};
float g_wxf[20]={0};
float g_wyf[20]={0};
int  g_num = 0;
int  g_num_max = 20;
float out[20]={0};
int g_out_num = 5;
float xs[] = {0,0,0,0,0,0,0};
float c[] = {1.58060653,2.8675266,2.35079111,2.07251069,0.81179641,0.3697686};
float ys[] = {0,0,0,0,0,0,0};
float as[] = {0,0,0,0,0,0};
float bs[] = {0,0,0,0,0,0};
float var[] = {0,0,0,0,0,0,0};

float dot(float *ax, float *bx,  int len)
{
	 int i;
	 float res = 0;
	/* 
	 memset(var,0,7);

	for(i = 0;i<len;i++)
	{
		var[i]= ax[i]*bx[i];
	}

	for( i = 0; i < len; i++) 
        res += var[i];
	*/
	for( i = 0; i < len; i++) 
	{
		
		res += ax[i]*bx[i];	
		//snav_info_print("dot[%d]=[%.8f][%.8f][%.8f]",i,ax[i],bx[i],res);
	}
        
  
	return res;
}

float iir_filter(float coldata[],float b[],float a[])  
{  
	float out_all = 0;
	int count = g_num_max;
	
	memset(out,0,20);
	
	for(int j = 0;j<7;j++)
	{
		xs[j] = 0;
		ys[j] = 0;
	}
	
	for(int i = 0;i<count;i++)
	{
		for(int ii = 0;ii<7;ii++)
		{
			as[ii] = 0;
			bs[ii] = 0;
		}
	
		for(int t = 0;t<6;t++)
			as[t] = xs[t];

		for(int tt = 1;tt<6;tt++)
			xs[tt] = as[tt-1];

		xs[0] = coldata[i];

		out[i] = dot(b,xs,7) - dot(c,ys,6);
		//snav_info_print("out[%d]=[%.8f][%.8f][%.8f]",i,coldata[i],out[i],dot(b,xs,7) - dot(c,ys,6));
		
		
		for(int ttt = 0;ttt<6-1;ttt++)
			bs[ttt] = ys[ttt];

		for(int tttt = 1;tttt<6-1;tttt++)
			ys[tttt] = bs[tttt-1];

		ys[0] = out[i];
	}
	
	for(int i = count - g_out_num;i<count;i++)
	{
		out_all += out[i];	
	}
	
	return out_all/g_out_num;  
} 


long getCurrentTime()    
{    
   struct timeval tv;    
   gettimeofday(&tv,NULL);    
   return tv.tv_sec * 1000 + tv.tv_usec / 1000;
   }

int mpu_driver_init(struct mpu9x50_config * config)
{
  snav_info_print("%s: Attempting to initialize MPU driver.",DRIVER_NAME);
  int mpu_init_ret = mpu9x50_initialize(config);
  if (mpu_init_ret != 0)
  {
    snav_error_print("%s: MPU failed to initialize[%d]",DRIVER_NAME,mpu_init_ret);
    return -1;
  }

  snav_info_print("%s: MPU SPI initialized successfully",DRIVER_NAME);

 // second_order_butterworth_lpf_init(500, 50, &lpf);//500HZ
  return 0;
}

int mpu_driver_close()
{
  int close_ret = mpu9x50_close();
  snav_info_print("%s: mpu close result: %d",DRIVER_NAME,close_ret);
  return close_ret;
}

int mpu_spi_read_data()
{
  //TODO: add reading / reporting MAG1


  struct mpu9x50_data sensor_data;
  int mpu_ret = mpu9x50_get_data(&sensor_data);

	float *axff;

  int bad_mpu_data = (sensor_data.accel_raw[0] == 0) && (sensor_data.accel_raw[1] == 0) && (sensor_data.accel_raw[2] == 0) &&
                     (sensor_data.gyro_raw[0]  == 0) && (sensor_data.gyro_raw[1]  == 0) && (sensor_data.gyro_raw[2]  == 0) ;

  if( (mpu_ret!=0) || (bad_mpu_data))
  {
    //FIXME: don't print too often
    snav_error_print("%s: Failed to read mpu data!",DRIVER_NAME);
    return -1;
  }

  float axf   = sensor_data.accel_raw[0]*sensor_data.accel_scaling/9.81;
  float ayf   = sensor_data.accel_raw[1]*sensor_data.accel_scaling/9.81;
  float azf   = sensor_data.accel_raw[2]*sensor_data.accel_scaling/9.81;
  float wxf   = sensor_data.gyro_raw[0]*sensor_data.gyro_scaling;
  float wyf   = sensor_data.gyro_raw[1]*sensor_data.gyro_scaling;
  float wzf   = sensor_data.gyro_raw[2]*sensor_data.gyro_scaling;
  
  if(g_num < g_num_max)
  {
		g_axf[g_num] = axf;
		g_ayf[g_num] = ayf;
		g_wxf[g_num] = wxf;
		g_wyf[g_num] = wyf;
		g_num++;
  }
  else
  {
	  for(int i = 0; i<g_num - 1  ; i++)
	  {
		  g_axf[i] = g_axf[i+1];
		  g_ayf[i] = g_ayf[i+1];
		  g_wxf[i] = g_wxf[i+1];
		  g_wyf[i] = g_wyf[i+1];
	  }
	  g_axf[g_num-1] = axf;
	  g_ayf[g_num-1] = ayf;
	  g_wxf[g_num-1] = wxf;
	  g_wyf[g_num-1] = wyf;
	  
	  snav_info_print("---%.8f  %.8f  %.8f         %.8f %.8f %.8f",axf,ayf,azf,wxf,wyf,wzf);

	  pthread_mutex_lock(&imu_lock);

	  imu_read_counter++;
	  imu_read_time  = sensor_data.timestamp;
	  imu_fresh_data = 1;
	  
	  
	  ax   = iir_filter(g_axf,a,b);
	  ay   = iir_filter(g_ayf,a,b);
	  //az   = iir_filter(azf,a,b);
	  az   = azf;
	  wx   = iir_filter(g_wxf,a,b);
	  wy   = iir_filter(g_wyf,a,b);
	  //wz   = iir_filter(wzf,a,b);
	  wz   = wzf;

	  temp = sensor_data.temperature;

	  pthread_mutex_unlock(&imu_lock);

	  snav_info_print("***%.8f  %.8f  %.8f         %.8f %.8f %.8f",ax,ay,az,wx,wy,wz);
	  snav_info_print("@@@");
  }

	

  return 0;
}