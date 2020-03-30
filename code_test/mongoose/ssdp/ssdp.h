#ifndef _SSDP_H_
#define _SSDP_H_

#define ETHNAME "eth0"
#define SSDP_ADDR "239.255.255.250"
#define SSDP_PORT 1900
#define DEVICE_NAME "synaptics-as-390"

long long get_current_time();
int send_msearch();
long long ssdp_send_msearch(long long last_time, int ms);
#endif
