/*************************************************************************
	> File Name: dlopen.cpp
	> Author: Rock
	> Mail: ruidongren@163.com 
	> Created Time: 2017年02月09日 星期四 09时19分45秒
 ************************************************************************/

#include<iostream>
#include <dlfcn.h> 
using namespace std; 

int main() 
{ 
void *dp; 

dp = dlopen( "libmag0_hmc5883l_driver.so ",RTLD_NOW); 
if (dp==NULL) 
{ 
	printf("open ok");
fputs(dlerror(),stderr); 
exit(1); 
} 

dlclose(dp); 
return 0; 
} 