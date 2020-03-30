#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ssdp.h"

#define SSDP_BUFFER_LEN 2048

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
        "\r\n",
        SSDP_ADDR, SSDP_PORT,
        DEVICE_NAME);

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

/*int main() {
    long long last_time = 0;
    printf("Dan get last_time %lld", last_time);
    for (;;) {
        printf("Dan get last_time %lld in main\n", last_time);
        last_time = ssdp_send_msearch(last_time, 5000);
    }
    return 0;
}*/
