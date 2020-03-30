#include "nmtool.h"
#include "nm-default.h"

#include "nmcli.h"
#include "nmtool.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <locale.h>
#include <glib-unix.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "nm-client-utils.h"

#include "nm-utils/nm-hash-utils.h"

#include "polkit-agent.h"
#include "utils.h"
#include "common.h"
#include "connections.h"
#include "devices.h"
#include "general.h"
#include "agent.h"
#include "settings.h"




#include <pthread.h>
#include <sys/time.h>


pthread_cond_t app_cv;
pthread_mutex_t app_lock;

typedef int (*cmd_handler)(int argc, char **arg, int ix);

typedef struct _input_cmd {
	char *cmd_name;
	cmd_handler func;
	char *help;
} input_cmd;


int cmd_intf_list(int argc, char **p, int ix);
int cmd_intf_status(int argc, char **p, int ix);
int cmd_cur_ssid(int argc, char **p, int ix);
int cmd_dhcp(int argc, char **p, int ix);
int cmd_ip_addr(int argc, char **p, int ix);
int cmd_subnet(int argc, char **p, int ix);
int cmd_gateway(int argc, char **p, int ix);
int cmd_dns(int argc, char **p, int ix);
int cmd_alt_dns(int argc, char **p, int ix);
int cmd_mac(int argc, char **p, int ix);
int cmd_wifi_scan(int argc, char **p, int ix);
int cmd_connect_to_ap(int argc, char **p, int ix);

static input_cmd input_cmds[] = {
	{ "intflist", 	cmd_intf_list, 	"\t List available interfaces" },
	{ "intfstatus", cmd_intf_status,"\t interface [up/down]   Get/Set  Current status of specified interface (up/down)" },
	{ "ssid", 	cmd_cur_ssid, 	"\t\t interface    return SSID if associated in case of wlan0 or SSID of uAP if enabled" },
	{ "dhcp", 	cmd_dhcp, 	"\t\t interface [on/off]    Get/Set dhcp mode" },
	{ "ipaddr", 	cmd_ip_addr, 	"\t\t interface [IP Address]   Get/Set IP address for interface " },
	{ "subnet", 	cmd_subnet, 	"\t\t interface [Subnet Mask]   Get/Set subnet mask for interface " },
	{ "gateway", 	cmd_gateway, 	"\t interface [gateway IP]   Get/Set gateway IP address for interface " },
	{ "dns", 	cmd_dns, 	"\t\t interface [DNS IP]   Get/Set dns IP address for interface" },
	{ "altdns", 	cmd_alt_dns, 	"\t\t interface [Alt DNS IP]   Get/Set alternate dns IP address for interface" },
	{ "mac", 	cmd_mac, 	"\t\t interface [MAC Address]   Get/Set MAC address for interface" },
	{ "scan", 	cmd_wifi_scan, 	"\t\t Performs a wifi scan on wlan0 interface" },
	{ "connect", 	cmd_connect_to_ap, "\t SSID passkey   Associate to specified AccessPoint (wlan0 interface) " },
	{ NULL, NULL, NULL }
};

static void print_usage(char *p)
{
	int ix = 0;
	printf("Usage: %s command [param] \n",p);
	printf("Available commands are: \n");
	while (NULL != input_cmds[ix].cmd_name) {
		printf("%s %s\n",input_cmds[ix].cmd_name,input_cmds[ix].help);
		ix++;
	}

	printf(" \n");
	return;
}

static void printf_fn_usage(int ix)
{
	printf("Usage: %s %s\n",input_cmds[ix].cmd_name,input_cmds[ix].help);
}

int read_input_no_newline(char *buf, int maxlen)
{
	char *p;
	fgets(buf,maxlen,stdin);
	if ( (p=strchr(buf,'\n')) != NULL )
		*p = '\0';
}

#define MAX_USR_INPUT 150
static int get_input(int *num_args, char *args[])
{
	char usr_input[MAX_USR_INPUT] = {0};
	char *p;
	char delimiters[] = " \t\r\n";
	int ix = 0;
	*num_args = 0;
	printf("btinfo > ");
	if (fgets(usr_input, MAX_USR_INPUT, stdin)) {
		usr_input[MAX_USR_INPUT-1] = 0;
		p = strtok(usr_input, delimiters);
		if (NULL != p) {
			strcpy(args[*num_args],p);
			(*num_args)++;
		}
		while (NULL != p) {
			p = strtok(NULL,delimiters);
			if ((*num_args < 3) && (NULL != p))
			{
				strcpy(args[*num_args],p);
				(*num_args)++;
			}
		}
	}
	else
	{
		printf("No string entered..");
		*num_args = 0;
		return 0;
	}

return 1;
}

int main(int argc, char *argv[])
{
	int ix,num_args,nret = -1;
	char *args[4];
	char buffer[1024] = {0};

	for (ix = 0; ix < 4; ix++) {
		args[ix] = buffer + ix*256;
	}

	nmtool_init();

	do {
		if ( 2 > argc) {
			print_usage(argv[0]);
			break;
		}

		if (0 == strcmp(argv[1],"help")) {
			print_usage(argv[0]);
			break;
		}

		if (0 == strcmp(argv[1],"quit"))
			break;

		for (ix = 0; NULL != input_cmds[ix].cmd_name; ix++) {
			if (0 == strcmp(argv[1],input_cmds[ix].cmd_name)) {
				nret = input_cmds[ix].func(argc,argv,ix);
			}
		}
	} while (0);
	nmtool_exit();

	return nret;
}

int cmd_intf_list(int argc, char **p, int ix)
{
	interface_t intf[15];
	interface_list intf_list;
	NMTOOL_STATUS nret;

	intf_list.num_entries = 15;
	intf_list.pintf = intf;

	nret = get_interface_list(&intf_list);
	if (NMTOOL_STATUS_SUCCESS == nret) {
		int ix;
		for (ix = 0; ix < intf_list.num_entries; ix++) {
			printf("  Device[%d]: %s state:%d\n",
					ix, intf_list.pintf[ix].intf_name,
					(up == intf_list.pintf[ix].state)?1:0);
		}
	}
	else {
		printf("%s():%d ERROR!\n",__func__,__LINE__);
	}
	return 1;
}

int cmd_intf_status(int argc, char **p, int ix)
{
	intf_state state;
	NMTOOL_STATUS nret;

	if (argc < 3) {
		printf("\n Incorrect param\n");
		printf_fn_usage(ix);
		return -1;
	}

	if (argc == 3) {
		nret = get_interface_status(p[2], &state);
		if (NMTOOL_STATUS_SUCCESS == nret) {
			printf(" %s state: %s\n",p[2],
				(up == state)?"up":"down");
		}
		else {
			printf("%s(): Error getting status\n",__func__);
		}
		return 0;
	}

	if (0 == strcmp("up",p[3])) {
		set_interface_status(p[2], (intf_state) up);
	}

	if (0 == strcmp("down",p[3])) {
		set_interface_status(p[2], (intf_state) down);
	}
	return 1;
}

int cmd_cur_ssid(int argc, char **p, int ix)
{
	NMTOOL_STATUS nret;
	nm_ssid ssid;

	if (argc < 3) {
		printf("\n Incorrect param\n");
		printf_fn_usage(ix);
		return -1;
	}

	if (argc == 3) {
		nret = get_SSID_for_interface(p[2], &ssid);
		if (NMTOOL_STATUS_SUCCESS == nret) {
			printf(" SSID is %s\n",ssid.ssid);
		}
		else {
			printf("%s(): Error getting ssid\n",__func__);
		}
		return 0;
	}

	return 1;
}

int cmd_dhcp(int argc, char **p, int ix)
{
	NMTOOL_STATUS nret;
	dhcp_state state;

	if (argc < 3) {
		printf("\n Incorrect param\n");
		printf_fn_usage(ix);
		return -1;
	}

	if (argc == 3) {
		nret = get_dhcpmode_for_interface(p[2], &state);
		if (NMTOOL_STATUS_SUCCESS == nret) {
			switch (state) {
				case dhcp_on:
					printf("DHCP state: ON\n");
					break;
				case dhcp_off:
					printf("DHCP state: OFF\n");
			}
		}
		else {
			printf("%s(): Error getting status\n",__func__);
		}
		return 0;
	}

	if (0 == strcmp("on",p[3])) {
		state = dhcp_on;
		set_dhcpmode_for_interface(p[2], state);
	}

	if (0 == strcmp("off",p[3])) {
		state = dhcp_off;
		set_dhcpmode_for_interface(p[2], state);
	}

	return 1;
}

int cmd_ip_addr(int argc, char **p, int ix)
{
	NMTOOL_STATUS nret;
	dhcp_state state;
	ip4addr addr;

	if (argc < 3) {
		printf("\n Incorrect param\n");
		printf_fn_usage(ix);
		return -1;
	}

	if (argc == 3) {
		nret = get_ip_address_for_interface(p[2], &addr);
		if (NMTOOL_STATUS_SUCCESS == nret) {
			printf(" ip address:%s\n",addr.ip);
		}
		else {
			printf("%s(): Error getting ip address\n",__func__);
		}
		return 0;
	}

	if (argc == 4) {
		if (strlen(p[3]) > 15) {
			printf("invalid ip address\n");
			return -1;
		}
		nret = set_ip_address_for_interface(p[2], p[3]);
		if (NMTOOL_STATUS_SUCCESS != nret) {
			printf("%s(): Error setting ip address\n",__func__);
		}
		return 0;
	}

	return 1;
}

int cmd_subnet(int argc, char **p, int ix)
{
	NMTOOL_STATUS nret;
	dhcp_state state;
	ip4addr addr;

	if (argc < 3) {
		printf("\n Incorrect param\n");
		printf_fn_usage(ix);
		return -1;
	}

	if (argc == 3) {
		nret = get_subnet_for_interface(p[2], &addr);
		if (NMTOOL_STATUS_SUCCESS == nret) {
			printf(" subnet mask:%s\n",addr.ip);
		}
		else {
			printf("%s(): Error getting subnet mask\n",__func__);
		}
		return 0;
	}

	if (argc == 4) {
		if (strlen(p[3]) > 15) {
			printf("invalid subnet\n");
			return -1;
		}
		nret = set_subnet_for_interface(p[2], p[3]);
		if (NMTOOL_STATUS_SUCCESS != nret) {
			printf("%s(): Error setting subnet mask\n",__func__);
		}
		return 0;
	}


	return 1;
}

int cmd_gateway(int argc, char **p, int ix)
{
	NMTOOL_STATUS nret;
	dhcp_state state;
	ip4addr addr;

	if (argc < 3) {
		printf("\n Incorrect param\n");
		printf_fn_usage(ix);
		return -1;
	}

	if (argc == 3) {
		nret = get_gateway_for_interface(p[2], &addr);
		if (NMTOOL_STATUS_SUCCESS == nret) {
			printf(" gateway:%s\n",addr.ip);
		}
		else {
			printf("%s(): Error getting gateway\n",__func__);
		}
		return 0;
	}

	if (argc == 4) {
		if (strlen(p[3]) > 15) {
			printf("invalid gateway\n");
			return -1;
		}
		nret = set_gateway_for_interface(p[2], p[3]);
		if (NMTOOL_STATUS_SUCCESS != nret) {
			printf("%s(): Error setting gateway\n",__func__);
		}
		return 0;
	}


	return 1;
}

int cmd_dns(int argc, char **p, int ix)
{
	NMTOOL_STATUS nret;
	dhcp_state state;
	ip4addr addr;

	if (argc < 3) {
		printf("\n Incorrect param\n");
		printf_fn_usage(ix);
		return -1;
	}

	if (argc == 3) {
		nret = get_dns_for_interface(p[2], &addr);
		if (NMTOOL_STATUS_SUCCESS == nret) {
			printf(" nameserver:%s\n",addr.ip);
		}
		else {
			printf("%s(): Error getting dns\n",__func__);
		}
		return 0;
	}

	if (argc == 4) {
		if (strlen(p[3]) > 15) {
			printf("invalid dns\n");
			return -1;
		}
		nret = set_dns_for_interface(p[2], p[3]);
		if (NMTOOL_STATUS_SUCCESS != nret) {
			printf("%s(): Error setting dns\n",__func__);
		}
		return 0;
	}


	return 1;
}

int cmd_alt_dns(int argc, char **p, int ix)
{
	NMTOOL_STATUS nret;
	dhcp_state state;
	ip4addr addr;

	if (argc < 3) {
		printf("\n Incorrect param\n");
		printf_fn_usage(ix);
		return -1;
	}

	if (argc == 3) {
		nret = get_alternate_dns_for_interface(p[2], &addr);
		if (NMTOOL_STATUS_SUCCESS == nret) {
			printf(" alternate nameserver:%s\n",addr.ip);
		}
		else {
			printf("%s(): Error getting alternate dns\n",__func__);
		}
		return 0;
	}

	if (argc == 4) {
		if (strlen(p[3]) > 15) {
			printf("invalid dns\n");
			return -1;
		}
		nret = set_alternate_dns_for_interface(p[2], p[3]);
		if (NMTOOL_STATUS_SUCCESS != nret) {
			printf("%s(): Error setting alternate dns\n",__func__);
		}
		return 0;
	}

	return 1;
}

int cmd_mac(int argc, char **p, int ix)
{
	NMTOOL_STATUS nret;
	dhcp_state state;
	macaddr addr;

	if (argc < 3) {
		printf("\n Incorrect param\n");
		printf_fn_usage(ix);
		return -1;
	}

	if (argc == 3) {
		nret = get_mac_address_for_interface(p[2], &addr);
		if (NMTOOL_STATUS_SUCCESS == nret) {
			printf(" mac address:%s\n",addr.mac);
		}
		else {
			printf("%s(): Error getting mac address\n",__func__);
		}
		return 0;
	}

	if (argc == 4) {
		if (strlen(p[3]) != 17) {
			printf("invalid mac address\n");
			return -1;
		}
		nret = set_mac_address_for_interface(p[2], p[3]);
		if (NMTOOL_STATUS_SUCCESS != nret) {
			printf("%s(): Error setting dns\n",__func__);
		}
		return 0;
	}


	return 1;
}

int cmd_wifi_scan(int argc, char **p, int ix)
{
	NMTOOL_STATUS nret;
	//nm_ssid ssids[99];
	nm_ssid *ssids;
	APList aplist;

	ssids = (nm_ssid*) malloc(99 * sizeof(nm_ssid));
	if (!ssids)
		return 1;
	aplist.num_entries = 99;
	aplist.pAPlist = ssids;

	if (argc < 3) {
		printf("\n Incorrect param\n");
		printf_fn_usage(ix);
		free(ssids);
		return -1;
	}

	if (argc == 3) {
		nret = get_wifi_scan_results(p[2], &aplist);
		if ((NMTOOL_STATUS_SUCCESS == nret) ||
			( NMTOOL_STATUS_INSUFFICIENT_BUFFER == nret)) {
			for (int ix = 0; ix < aplist.num_entries; ix++)
			{
				printf(" Idx[%d]: %s\n", ix, aplist.pAPlist[ix].ssid);
			}
			if ( NMTOOL_STATUS_INSUFFICIENT_BUFFER == nret )
				printf(" WARNING! Insufficient buffer; more APs found\n");
		}
		else {
			printf("%s(): Error getting scan results ret:%d\n",__func__,nret);
		}
		free(ssids);
		return 0;
	}



	free(ssids);
	return 1;
}

int cmd_connect_to_ap(int argc, char **p, int ix)
{
	NMTOOL_STATUS nret;
	nm_ssid ssids[99];
	APList aplist;

	aplist.num_entries = 99;
	aplist.pAPlist = ssids;

	if (argc < 4) {
		printf("\n Incorrect param\n");
		printf_fn_usage(ix);
		return -1;
	}

	if (argc == 4) {
		nret = connect_to_access_point("wlan0", p[2], p[3]);

		if (NMTOOL_STATUS_SUCCESS == nret) {
			printf(" Connect to AP success!\n");
		}
		else {
			printf("%s(): Error getting scan address\n",__func__);
		}
		return 0;
	}
	return 1;
}

