#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#include "ssdp.h"

#define SSDP_BUFFER_LEN 2048

char  buffer[120];
char  recv_from[120];
char code_null[] ="null";


char  *get_code(){	
	int fd,size;
    fd=open("/data/avs/code.cfg",O_RDONLY);
	if(fd >0)
	{
		size=read(fd,buffer,sizeof(buffer));
		close(fd);
		printf("%s",buffer);
		if(strlen(buffer)<2)
			return code_null;
		else
		{
			//remove("/data/avs/code.cfg");
			return buffer;
		}
	}else{
		return code_null;
	}
}

long long get_current_time() {
    struct timeval time;
    if (gettimeofday(&time, NULL) == -1) {
        printf("gettimeofday failed, errno = %s (%d)\n", strerror(errno), errno);
        return -1;
    }
    return (long long) time.tv_sec * 1000 + (long long) time.tv_usec / 1000;
}


int send_msearch() {
    int result = -1;
    char data[SSDP_BUFFER_LEN] = {};
    snprintf(data, sizeof(data),
        "M-SEARCH * HTTP/1.1\r\n"
        "HOST:%s:%d\r\n"
        "MAN:\"ssdp:discover\"\r\n"
        "MX:1\r\n"
        "ST:urn:%s\r\n"
		"Code:%s\r\n"
        "\r\n",
        SSDP_ADDR, SSDP_PORT,
        DEVICE_NAME,get_code());

    size_t data_len = strlen(data);

    // 1. create UDP socket
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        printf("create socket failed, errorno = %s (%d)\n", strerror(errno), errno);
        goto end;
    }

    // 2. disable IP_MULTICAST_LOOP
    char opt = 0;
    if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &opt, sizeof(opt)) < 0) {
        printf("setsockopt IP_MULTICAST_LOOP failed, errno = %s (%d)\n", strerror(errno), errno);
        goto end;
    }

    // 3. set destination address
    struct sockaddr_in dest_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(SSDP_PORT)
    };
    if (inet_aton(SSDP_ADDR, &dest_addr.sin_addr) == 0) {
        printf("inet_aton failed, errno = %s (%d)\n", strerror(errno), errno);
        goto end;
    }


    // 4. send data
    if (sendto(fd, data, data_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1) {
        printf("send data failed, errno = %s (%d)\n", strerror(errno), errno);
        goto end;
    }
/*
len = recvfrom(socket, buffer, sizeof(buffer), 0, &addr &addr_len);


    sendto(sockfd, buffer, len, 0, &addr, addr_len);
*/
	//int len = recvfrom(fd, recv_from, sizeof(recv_from), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	//printf("=== rock=== recvfrom data   recv_from= %s \n", recv_from);



    result = 0;

    end:
        if (fd >= 0 && close(fd) != 0) {
            printf("close fd failed, errno = %s (%d)\n", strerror(errno), errno);
        }
        return result;
}

long long ssdp_send_msearch(long long last_time, int ms) {
    long long current_time = get_current_time();
    if (current_time < 0) {
        printf("got invalid timestamp %lld\n", current_time);
        return 0;
    }
    if (current_time - last_time >= ms) {
        send_msearch();
        return current_time;
    }
    return last_time;
}

int main() {
    long long last_time = 0;
    printf("Dan get last_time %lld", last_time);
    for (;;) {
		sleep(1);
        printf("Dan get last_time %lld in main\n", last_time);
        last_time = ssdp_send_msearch(last_time, 50000000);
    }
    return 0;
}
