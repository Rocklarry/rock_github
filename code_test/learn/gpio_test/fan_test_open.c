/*************************************************************************
	> File Name: gpio_test.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2018年01月03日 星期三 09时47分17秒
 ************************************************************************/

#include <stdio.h> 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>   //define O_WRONLY and O_RDONLY  
 
//控制引脚: GPIO_112
#define SYSFS_GPIO_EXPORT           "/sys/class/gpio/export"
#define SYSFS_GPIO_SFILE_115			"/sys/class/gpio/gpio115"
#define SYSFS_GPIO_RST_PIN_VAL_115      "115"   
#define SYSFS_GPIO_RST_DIR_115          "/sys/class/gpio/gpio115/direction"
#define SYSFS_GPIO_RST_DIR_VAL      "out"  
#define SYSFS_GPIO_RST_VAL_115          "/sys/class/gpio/gpio115/value"
#define SYSFS_GPIO_RST_VAL_H        "1"
#define SYSFS_GPIO_RST_VAL_L        "0"

#define GPIO_ON						"on"
#define GPIO_OFF					"off"

/*
判断文件是否存在 
不存在 0
存在 1
*/
int is_file_exist(const char *file_path)
{
	if(file_path == NULL)
		return 0;
	if(access(file_path,F_OK) ==0 )
		return 1;
	return 0;
}
 

// 打开风扇 fan—test on
// 关闭风扇 fan—test off

int main(int argc, char **argv) 
{ 
	int fd;
	int res;

	//判断接口是否存在

	if (!is_file_exist(SYSFS_GPIO_SFILE_115))
	{
		//打开端口/sys/class/gpio
         fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
         if(fd == -1)
         {
			printf("ERR: Radio hard reset pin open error.\n");
                   return -1;
         }
		 //# echo 112 > export
         write(fd, SYSFS_GPIO_RST_PIN_VAL_115 ,sizeof(SYSFS_GPIO_RST_PIN_VAL_115)); 
		close(fd); 
	
		//设置端口方向/sys/class/gpio/gpio112# echo out > direction
		fd = open(SYSFS_GPIO_RST_DIR_115, O_WRONLY);//打开direction
         if(fd == -1)
         {
			printf("ERR: Radio hard reset pin direction open error.\n");
			return -1;
         }
		 write(fd, SYSFS_GPIO_RST_DIR_VAL, sizeof(SYSFS_GPIO_RST_DIR_VAL));
		 close(fd); 
		 printf(" Determines that the file does not exist, creates and sets the output\n");
	}

		fd = open(SYSFS_GPIO_RST_VAL_115, O_WRONLY);
         if(fd == -1)
         {
			printf("ERR: Radio hard reset pin open error.\n");
			return -1;
         }

		 res = write(fd, SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H+1));
		 if(res < 0)
		{
			 printf("write error \n");
		 }
		 close(fd);

		 printf(" pull up \n");
	
	return 0;
 
}  
