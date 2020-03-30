#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>
#include "nmtool.h"
#include "network.h"

char* get_macaddr(char *interface) {
   
    return "11.22.33.44.55.66";
}

char* get_current_ssid(char *interface) {

    return "123456";
}

char* get_ap_list(char *interface) {

    return "1234567";
}

int connect_wifi(char *interface, char* ssid, char* password)
{

	if (test_connection() != 0) {
			return 1;
		}
        return 0;

}

int get_ip(char* interface)
{
    return 1;
}

void down_hotspot()
{
    char down_hotspot_cmd[23] = "nmcli con down hotspot";
    puts(&down_hotspot_cmd);
    system(down_hotspot_cmd);
	printf(" ===rock===  %s cmd=%s \n",__func__,down_hotspot_cmd);
}

void up_hotspot()
{
    char up_hotspot_cmd[21] = "nmcli con up hotspot";
    puts(&up_hotspot_cmd);
    system(up_hotspot_cmd);
	printf(" ===rock===  %s  cmd=%s \n",__func__,up_hotspot_cmd);
}

int test_connection()
{
  int res = 1;
  int tried = 0;
  char *hostname;
  struct hostent * hostinfo;

  hostname = "amazon.com";
  while (tried < 20) {
    printf("===rock=== Test connection %d times\n", tried);
    hostinfo = gethostbyname(hostname);
    if (hostinfo != NULL) {
      res = 0;
      break;
    }
    usleep(1000000);
    tried++;
  }
  return res;
}
