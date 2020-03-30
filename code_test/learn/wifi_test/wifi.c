/*************************************************************************
	> File Name: wifi.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2017年10月20日 星期五 11时17分21秒
 ************************************************************************/

//#include <cutils/properties.h>
#include <ctype.h>
#include <dlfcn.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/prctl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#define READ_BUF_SIZE 1024
#define SIZE_1K 1024
static int finish_wifi_test = 0;
static int wifi_test_result = 0;
static int wifi_test_error = 0;
static pthread_t processThreadPid;

#define   FAILED  -1
#define   SUCCESS  0

#define KEY_WORD_CHANNEL "Channel:"
#define KEY_WORD_ESSID "SSID:"
#define KEY_WORD_SIGNAL_LEVEL "Signal level"


//static const char* tag = LOG_TAG;

static pthread_mutex_t test_mutex;     /* Only one request at a time */
static pthread_cond_t test_cond;       /* Waiting for RESP message */



bool check_file_exist(const char *path) {
    struct stat buffer;
    int err = stat(path, &buffer);

    if(err == 0)
        return true;
    if(errno == ENOENT) {
        printf("file(%s) do not exist\n", path);
        return false;
    }
    return false;
}


void local_exe_cmd(const char *cmd, char *result, int size)
{
    char line[1024];
    char ps[1024] = {0};
    FILE *pp;
    printf("local_exe_cmd cmd = %s\n",cmd);
    snprintf(ps, cmd, sizeof(ps));
    if((pp = popen(ps, "r")) != NULL)
    {
        printf("begain gets");
        while(fgets(line, sizeof(line), pp) != NULL)
        {
            if(strlen(line) > 1)
                snprintf(result, line, size);
            printf("%s",line);
        }
        printf("close the pipe \n");
        pclose(pp);
        pp = NULL;
    } else
    {
            printf( "popen %s error\n", ps);
    }
}

void signalHandler(int signal)
{
    printf("pthread_exit");
    pthread_exit(NULL);
}

void kill_pid_by_name( const char* pidName)
{
    DIR *dir;
    struct dirent *next;
    long pidList=0;
    char cmd[20];

    dir = opendir("/proc");
    if (!dir)
    {
        //    perror_msg_and_die("Cannot open /proc");
        printf("cannot open /proc");
    }

    while ((next = readdir(dir)) != NULL)
    {
        FILE *status;
        char filename[READ_BUF_SIZE];
        char buffer[READ_BUF_SIZE];
        char name[READ_BUF_SIZE];
        memset(buffer, 0, sizeof(buffer));

        /* Must skip ".." since that is outside /proc */
        if (strcmp(next->d_name, "..") == 0)
            continue;

        /* If it isn't a number, we don't want it */
        if (!isdigit(*next->d_name))
            continue;

        sprintf(filename, "/proc/%s/status", next->d_name);
        if (! (status = fopen(filename, "r")) )
        {
            continue;
        }
        if (fgets(buffer, READ_BUF_SIZE-1, status) == NULL)
        {
            fclose(status);
            continue;
        }
        fclose(status);

        /* Buffer should contain a string like "Name:   binary_name" */
        if (buffer[0] != '\0')
            sscanf(buffer, "%*s %s", name);
        if (strcmp(name, pidName) == 0)
        {
            pidList = strtol(next->d_name, NULL, 0);
            sprintf(cmd, "kill %ld", pidList);
            system(cmd);
        }
    }
}

void stop_wifi()
{
    kill_pid_by_name("iw");
}

static void load_driver() {
    char result[1024 * 10] = {0};
    local_exe_cmd("insmod /system/lib/modules/wlan.ko", result, sizeof(result));
}

static void unload_driver() {
    char result[1024 * 10] = {0};
    local_exe_cmd("rmmod wlan", result, sizeof(result));
}

static void config_wlan() {
    char result[1024 * 10] = {0};
    local_exe_cmd("ifconfig wlan0 up", result, sizeof(result));
}


void test_wifi_func()
{
    struct timeval before;
    struct timeval after;
    char result[1024 * 10] = {0};
    char cmdstr[256] = {0};
    int i = 0;

    while (i < 2)
    {
        printf("begin %d times searching...\n", i+1);
        local_exe_cmd("iw dev wlan0 scan", result, sizeof(result));
        char* p = result;
        if (*p != '\0')
        {
                if(strstr(p,KEY_WORD_ESSID) != NULL)
                {
                printf("find wifi ssid\n");
                wifi_test_result = 1;
                pthread_cond_signal( &test_cond );
                break;
                }
                else
                {
                        printf("%s",p);
                }
        }
        i++;
    }
    if (wifi_test_result == 0)
    {
        printf("can't find wifi \n");
    }
}

static int start_test() {
    if(!check_file_exist("/system/lib/modules/wlan.ko") || !check_file_exist("/system/bin/iw"))
        return FAILED;

    load_driver();
    config_wlan();

    test_wifi_func();

    unload_driver();
    if(wifi_test_result) {
        printf("wifi test pass\n");
    } else {
        printf("wifi test fail\n");
    }

    return wifi_test_result ;
}

void *processThread_bt(void* parm)
{
    signal(SIGUSR1, signalHandler);
    start_test();
    return NULL;
}

int get_wifi_result()
{
    printf("get wifi test result = %d\n", wifi_test_result);
    return wifi_test_result;
}

int get_wifi_status()
{
    printf("get wifi test status = %d\n", finish_wifi_test);
    return finish_wifi_test;
}
void end_test_wifi()
{
    printf("end test wifi\n");
    stop_wifi();
    pthread_kill(processThreadPid, 0);
    wifi_test_result = 0;
    finish_wifi_test = 0;
}

int main(int argc, char **argv)
{
        start_test();
}

