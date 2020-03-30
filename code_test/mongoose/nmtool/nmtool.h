#ifndef __NMTOOL_H__
#define __NMTOOL_H__

typedef enum _intf_state {up, down} intf_state;
typedef enum _dhcp_state {dhcp_on, dhcp_off} dhcp_state;

typedef enum _NMTOOL_STATUS {
	NMTOOL_STATUS_SUCCESS,
	NMTOOL_STATUS_ERROR,
	NMTOOL_STATUS_ERR_INTERFACE,
	NMTOOL_STATUS_ERR_INTERFACE_DOWN,
	NMTOOL_STATUS_NO_IPADDRESS,
	NMTOOL_STATUS_NO_MACADDRESS,
	NMTOOL_STATUS_INSUFFICIENT_BUFFER,
	NMTOOL_STATUS_INVALID_PARAM,
	NMTOOL_STATUS_NO_ALT_DNS
} NMTOOL_STATUS;



typedef struct _interface_t {
	char intf_name[15];
	intf_state state;
} interface_t;

typedef struct _interface_list {
	int num_entries;
	interface_t *pintf;
} interface_list;

typedef struct _nm_ssid {
	char ssid[33];
} nm_ssid;

typedef struct _APList {
	int num_entries;
	nm_ssid *pAPlist;
} APList;

typedef struct _ip4addr {
	char ip[16];
} ip4addr;

typedef struct _macaddr {
	char mac[18];
} macaddr;

NMTOOL_STATUS nmtool_init();
NMTOOL_STATUS nmtool_exit();

// Get a list of interfaces currently managed by
// network manager.
// param: interface_list [inout] Preallocated list large enough to
//           hold all potential interfaces
NMTOOL_STATUS get_interface_list(interface_list *list);

// Retrieves the current state of the specified interface
// param1: interface [in]  name of the interface
// param2: state [out]  current state of the interface
NMTOOL_STATUS get_interface_status(char *interface, intf_state *state);

// Change state of the specified interface
// param1: interface [in]  name of the interface
// param2: state [in]  new state of the interface
NMTOOL_STATUS set_interface_status(char *interface, intf_state state);

// Get SSID associated with wireless device
// param1: interface [in] name of interface
// param2: ssid [out] ssid of the interface
NMTOOL_STATUS get_SSID_for_interface(char *interface, nm_ssid *ssid);

// Get dhcp mode of interface
// param1: interface [in] name of interface
// param2: state [out] dhcp mode of the interface
NMTOOL_STATUS get_dhcpmode_for_interface(char *interface, dhcp_state *state);

// Set dhcp mode of interface
// param1: interface [in] name of interface
// param2: state [out] dhcp mode of the interface
NMTOOL_STATUS set_dhcpmode_for_interface(char *interface, dhcp_state state);

NMTOOL_STATUS get_ip_address_for_interface(char *interface, ip4addr *addr);
NMTOOL_STATUS set_ip_address_for_interface(char *interface, ip4addr *addr);

NMTOOL_STATUS get_subnet_for_interface(char *interface, ip4addr *addr);
NMTOOL_STATUS set_subnet_for_interface(char *interface, ip4addr *addr);

NMTOOL_STATUS get_gateway_for_interface(char *interface, ip4addr *addr);
NMTOOL_STATUS set_gateway_for_interface(char *interface, ip4addr *addr);

NMTOOL_STATUS get_dns_for_interface(char *interface, ip4addr *addr);
NMTOOL_STATUS set_dns_for_interface(char *interface, ip4addr *addr);

NMTOOL_STATUS get_alternate_dns_for_interface(char *interface, ip4addr *addr);
NMTOOL_STATUS set_alternate_dns_for_interface(char *interface, ip4addr *addr);

NMTOOL_STATUS get_mac_address_for_interface(char *interface, macaddr *addr);
NMTOOL_STATUS set_mac_address_for_interface(char *interface, macaddr *addr);

NMTOOL_STATUS get_wifi_scan_results(char *interface, APList *plist);
NMTOOL_STATUS connect_to_access_point(char *interface, char *ssid, char *psk);

#endif //__NMTOOL_H__
