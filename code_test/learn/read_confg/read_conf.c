#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int get_conf_value(char *file_path, char *key_name, char *value)
{
    FILE *fp = NULL;
        char *line = NULL, *substr = NULL;
        size_t len = 0, tlen = 0;
        ssize_t read = 0;
    
    if(file_path == NULL || key_name == NULL || value == NULL)
    {
        printf("paramer is invaild!\n");
        return -1;
    }
        fp = fopen(file_path, "r");
        if (fp == NULL)
    {
        printf("open config file is error!\n");
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
                printf("config file format is invaild tlen is %d len is %d\n", tlen, len);
                tlen = strlen(value);
                printf("get value is %s tlen is %d\n", value, tlen);
                //replace enter key
                *(value+tlen-1) = '\0';
                break;
            }
            else
            {
                printf("config file format is invaild tlen is %d len is %d\n", tlen, len);
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

int main()
{
    char getva[128] = {0};
    char pathname_key[] = "Path";
    char profilename[] = "/home/idea/code_test/read_confg/camera.cfg";
    int ret;
	 ret= get_conf_value(profilename, pathname_key, getva);
    if(ret == 0)
        printf("get pathname_key's value from profile:%s is %s\n\n", profilename, getva);

	 ret = get_conf_value(profilename, "Name", getva);
    if(ret == 0)
        printf("get pathname_key's value from profile:%s is %s\n\n", profilename, getva);

	 ret = get_conf_value(profilename, "Frequency", getva);
    if(ret == 0)
        printf("get pathname_key's value from profile:%s is %s\n\n", profilename, getva);
    return ret;
}
