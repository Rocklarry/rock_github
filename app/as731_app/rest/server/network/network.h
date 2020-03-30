#ifndef _NETWORK_H_
#define _NETWORK_H_

char* get_macaddr(char *interface);
char* get_current_ssid(char *interface);
char* get_ap_list();
int connect_wifi(char *interface, char* ssid, char* password);
void refresh_interface(char* interface);
#endif
