/*************************************************************************
	> File Name: frequency.c
	> Author: Rock
	> Mail: ruidongren@163.com 
	> Created Time: 2017年04月01日 星期六 15时56分31秒
 ************************************************************************/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
int get_conf_value(char *file_path, char *key_name, char *value)
{
    FILE *fp = NULL;
        char *line = NULL, *substr = NULL;
        size_t len = 0, tlen = 0;
        size_t read = 0;
    
    if(file_path == NULL || key_name == NULL || value == NULL)
    {
        //printf("paramer is invaild!\n");
		syslog(LOG_INFO,"===rrd== paramer is invaild!\n");
        return -1;
    }
        fp = fopen(file_path, "r");
        if (fp == NULL)
    {
        //printf("open config file is error!\n");
		syslog(LOG_INFO,"===rrd== open config file is error!\n");
        return -1;
    }

        while ((read = getline(&line, &len, fp)) != -1) 
    {
        substr = strstr(line, key_name);
        if(substr == NULL)
        {
            continue;
        }
        else
        {
            tlen = strlen(key_name);
            if(line[tlen] == '=')
            {
                strncpy(value, &line[tlen+1], len-tlen+1);
                printf("config file format is invaild tlen is %d len is %d\n", (int)tlen, (int)len);
                tlen = strlen(value);
                printf("get value is %s tlen is %d\n",  value, (int)tlen);
                //replace enter key
                *(value+tlen-1) = '\0';
                break;
            }
            else
            {
                printf("config file format is invaild tlen is %d len is %d\n", (int)tlen, (int)len);
                fclose(fp);
                return -2;
            }
        }
        }
    if(substr == NULL)
    {
        printf("key: %s is not in config file!\n", key_name);
        fclose(fp);
        return -1;
    }

        free(line);
    fclose(fp);
    return 0;
}

int	frequency()
{
	char getva[128] = {0};
    char pathname_key[] = "Frequency";
    char profilename[] = "/etc/camera.cfg";

    int ret= get_conf_value(profilename, pathname_key, getva);
    if(ret == 0)
		{
       syslog(LOG_INFO," ===rrd===  get pathname_key's value from profile:%s is %s\n\n", profilename, getva);
		if(memcmp(getva,"60Hz",strlen("60Hz"))==0)
		return 1;
		else
		return 0;
		}
		else
		return 0;
}
int main(void)
{
	
 FILE *fp;
    fp = fopen("test.c","w"); // 已文件后缀名为.c,"w"方式打开文件
	fp = fopen("/tmp/test.c","w");
    fprintf(fp,"%s","#include <stdio.h>\n");
    fprintf(fp,"%s","int main(void)\n");
    fprintf(fp,"%s","{\n");
    fprintf(fp,"%s","printf(\"hello World!\");\n");
    fprintf(fp,"%s","}\n");
    fprintf(fp,"%s","return 0;\n");
    fclose(fp);

if(frequency())
	{
	//setAntibanding(ANTIBANDING_60HZ);
	syslog(LOG_INFO,"===rrd=== ANTIBANDING 60HZ");
	}
	else
	{
	//setAntibanding(ANTIBANDING_50HZ);
	syslog(LOG_INFO,"===rrd=== ANTIBANDING 50HZ");
	}

return 0;
}
