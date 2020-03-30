/*************************************************************************
	> File Name: main.c
	> Author: Rock
	> Mail: ruidongren@163.com 
	> Created Time: 2017年02月08日 星期三 17时45分54秒
 ************************************************************************/

#include<stdio.h>
#include <dlfcn.h>

int main(int argc,char *argv[])

{

    char module_name[1024]={"./libmag0_hmc5883l_driver.so"};

    char *error=NULL;

    void *module_handle = dlopen(module_name, RTLD_NOW);

    if(module_handle == NULL)

    {

      error = dlerror();

      char tp[1024] = {0}; 

      snprintf(tp, 256,

    "Load module \"%s\" error: %s\n",

    module_name, error);   

      printf("%s",tp);

      return 1;

    }else

    {

        printf("Load module[%s] success!\r\n",module_name);

    }

    return 1;

}