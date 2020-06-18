#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>
#include "nmtool.h"
#include "network.h"

char* get_macaddr(char *interface) {
    NMTOOL_STATUS nret;
    macaddr addr;
    char *mac_addr;
    mac_addr = (char *)malloc(sizeof(char) * 18);
    if (!mac_addr)
        return NULL;

    nmtool_init();
    nret = get_mac_address_for_interface(interface, &addr);
    nmtool_exit();
    if (NMTOOL_STATUS_SUCCESS == nret) {
        strcpy(mac_addr, addr.mac);
    }
    else {
        printf("%s(): Error getting mac_address for %s\n",__func__, interface);
        free(mac_addr);
        return NULL;
    }
	printf("===rock=== %s interface=%s mac_addr=%s addr.mac=%s \n",__func__,interface,mac_addr,addr.mac);
    return mac_addr;
}

char* get_current_ssid(char *interface) {
    NMTOOL_STATUS nret;
    nm_ssid s_ssid;

    char *ssid = (char *) malloc(sizeof(char) * 33);
    if (!ssid)
        return NULL;

    nmtool_init();
    nret = get_SSID_for_interface(interface, &s_ssid);
    nmtool_exit();
    if (NMTOOL_STATUS_SUCCESS == nret) {
        printf("Get current ssid %s", s_ssid.ssid);
        strcpy(ssid, s_ssid.ssid);
        printf("Copy current ssid %s", ssid);
    }
    else {
        printf("%s(): Error getting ssid for %s\n",__func__, interface);
        free(ssid);
        return NULL;
    }
	printf("===rock=== %s interface=%s ssid=%s s_ssid.ssid=%s \n",__func__,interface,ssid,s_ssid.ssid);
    return ssid;
}

char* get_ap_list(char *interface) {
    NMTOOL_STATUS nret;
    APList aplist;
    int max_ssids_num = 99;

    nm_ssid *s_ssids;
    s_ssids = (nm_ssid*) malloc( max_ssids_num * sizeof(nm_ssid));
    if (!s_ssids)
        return NULL;

    aplist.num_entries = max_ssids_num;
    aplist.pAPlist = s_ssids;

    //new_size = 33(nm_ssid.ssid) + 3("",)
    size_t new_size = 36;
    size_t current_size = 1;
    char * ap_list = malloc(1);
    if (!ap_list) {
        free(s_ssids);
        return NULL;
    }
    else
        *ap_list = '\0';

    nmtool_init();
    nret = get_wifi_scan_results(interface, &aplist);
    if ((NMTOOL_STATUS_SUCCESS == nret) || (NMTOOL_STATUS_INSUFFICIENT_BUFFER == nret)) {
        for (int i = 0; i < aplist.num_entries; i++) {
            printf("===rock=== Get ssid: %s\n", aplist.pAPlist[i].ssid);
            current_size = current_size + new_size;
            ap_list = (char *) realloc(ap_list, current_size);
            strcat(ap_list, "\"");
            strcat(ap_list, aplist.pAPlist[i].ssid);
            strcat(ap_list, "\",");
        }
        if ( NMTOOL_STATUS_INSUFFICIENT_BUFFER == nret )
            printf(" WARNING! Insufficient buffer; more APs found\n");
    }
    else {
        printf("%s(): Error getting ap_list for %s\n",__func__, interface);
        nmtool_exit();
        free(s_ssids);
        free(ap_list);
        return NULL;
    }
    free(s_ssids);
    nmtool_exit();
    return ap_list;
}

int connect_wifi(char *interface, char* ssid, char* password)
{
    NMTOOL_STATUS nret;
    printf("===rock=== In connect_wifi function ssid =%s password=%s\n", ssid,password);
    nmtool_init();
    nret = connect_to_access_point(interface, ssid, password);
    nmtool_exit();
    if (NMTOOL_STATUS_SUCCESS == nret) {
        printf(" Connect to AP success!\n");
        down_hotspot();
        if (test_connection() != 0) {
          up_hotspot();
          return 1;
        }
        return 0;
    }
    else {
        printf("%s(): Error connecting to AP\n",__func__);
        return 1;
    }
    return 1;
}

int get_ip(char* interface)
{
    NMTOOL_STATUS nret;
    ip4addr addr;
    nmtool_init();
    nret = get_ip_address_for_interface(interface, &addr);
    if (NMTOOL_STATUS_SUCCESS == nret) {
        down_hotspot();
        printf(" IP is %s!\n", addr.ip);
        nmtool_exit();
        return 0;
    }
    else {
        printf("%s(): Error getting IP\n",__func__);
        nmtool_exit();
        return 1;
    }
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
