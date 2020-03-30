

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <locale.h>
#include <glib-2.0/glib-unix.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "settings.h"
#include "nmtool.h"


typedef struct _ConnCBdata {
	NMRemoteConnection *prcon;
	GMainLoop *ploop;
} ConnCBdata;

NmCli nm_cli;

NMTOOL_STATUS nmtool_get_access_point(char *interface, NMDevice *pnmdev, char *ssid, NMAccessPoint **ppnmap);
static NMTOOL_STATUS nmtool_get_nmconn_from_interface(char *interface, NMConnection **ppcon, NMRemoteConnection **pprcon);

void _nm_utils_monotonic_timestamp_initialized (const struct timespec *tp,
                                             gint64 offset_sec,
                                             gboolean is_boottime){
}

void nmc_do_init(NmCli *nmc)
{
	nmc->client = NULL;
}

void nmc_do_cleanup (NmCli *nmc)
{
	g_clear_object (&nmc->client);
}

static void connection_add_and_activate_cb(GObject *client, GAsyncResult *result, gpointer *pdat)
{
	ConnCBdata *pcbdat = (ConnCBdata *) pdat;
	GError *perr = NULL;

	if (!nm_client_add_and_activate_connection_finish (nm_cli.client, result, &perr)) {
		g_error_free(perr);
	}
	g_main_loop_quit(pcbdat->ploop);
}

static void connection_activate_cb(GObject *client, GAsyncResult *result, gpointer *pdat)
{
	ConnCBdata *pcbdat = (ConnCBdata *) pdat;
	GError *perr = NULL;

	if (!nm_client_activate_connection_finish (nm_cli.client, result, &perr)) {
		g_error_free(perr);
	}
	g_main_loop_quit(pcbdat->ploop);
}

NMDeviceState get_device_state(char* pinterface)
{
	NMDevice *ndev;
	NMDeviceState dev_state = NM_DEVICE_STATE_UNKNOWN;
	ndev = nm_client_get_device_by_iface(nm_cli.client,pinterface);
	if (ndev) {
		dev_state = nm_device_get_state(ndev);
	}
	else {
		//printf(" ERROR! wlan0 device not found!!\n");
	}
	return dev_state;
}

NMTOOL_STATUS nmtool_init()
{
	GError *error = NULL;
	nm_g_type_init ();

	nmc_do_init(&nm_cli);

	nm_cli.client = nm_client_new(NULL,&error);
	if (!nm_cli.client) {
		printf("%s():%d nm_client_new() returned error: %s \n",__func__,__LINE__,error->message);
		nmc_do_cleanup(&nm_cli);
		return -1;
	}

	return 0;
}

NMTOOL_STATUS nmtool_exit()
{
	nmc_do_cleanup(&nm_cli);
	return 0;
}

static NMTOOL_STATUS nmtool_get_active_connection(char *uuid, NMActiveConnection **pnm_acon)
{
	GPtrArray *pActiveConns;
	int ix;
	char *cur_uuid;

	pActiveConns = nm_client_get_active_connections(nm_cli.client);
	for (ix = 0; ix < pActiveConns->len; ix++) {
		*pnm_acon = (NMActiveConnection *) pActiveConns->pdata[ix];
		if (NULL == *pnm_acon)
			continue;
		cur_uuid = nm_active_connection_get_uuid(*pnm_acon);
		if (0 == strcmp(uuid, cur_uuid)) {
			return NMTOOL_STATUS_SUCCESS;
		}

	}

	*pnm_acon = NULL;
	return NMTOOL_STATUS_ERROR;
}


NMTOOL_STATUS get_interface_list(interface_list *list)
{
	const GPtrArray *devices;
	int ix,iy;
	NMDeviceState dev_state = NM_DEVICE_STATE_UNKNOWN;
	const char *nmintf;
	gboolean managed;
	int list_size = list->num_entries;

	devices = nm_client_get_devices(nm_cli.client);

	if (devices)
	{
		list->num_entries = iy = 0;
		for(ix = 0; ix < devices->len; ix++)
		{
			NMDevice *nmd =  (NMDevice *) devices->pdata[ix];
			nmintf = nm_device_get_iface(nmd);
			dev_state = nm_device_get_state(nmd);
			managed = nm_device_get_managed(nmd);
			//printf("Device[%d]: intfname:%s state:%d managed:%d\n",
			//		ix,nmintf,dev_state,managed);
			if ((TRUE == managed) &&
			   (list->num_entries < list_size)){
				if (NM_DEVICE_STATE_ACTIVATED == dev_state)
					list->pintf[iy].state = up;
				else
					list->pintf[iy].state = down;
				strncpy(list->pintf[iy].intf_name,nmintf,14);
				list->pintf[iy].intf_name[14] = 0;
				list->num_entries++;
				iy++;
			}
		}
	}
	else
	{
		printf("%s():%d Error getting devices!\n",__func__,__LINE__);
		return NMTOOL_STATUS_ERROR;
	}
	return NMTOOL_STATUS_SUCCESS;
}


NMTOOL_STATUS get_interface_status(char *interface, intf_state *state)
{
	if (NULL == interface)
		return NMTOOL_STATUS_ERR_INTERFACE;
	NMDevice *ndev;
	NMDeviceState dev_state = NM_DEVICE_STATE_UNKNOWN;
	ndev = nm_client_get_device_by_iface(nm_cli.client,interface);
	if (ndev) {
		dev_state = nm_device_get_state(ndev);
		*state = (NM_DEVICE_STATE_ACTIVATED == dev_state)?up:down;
	}
	else {
		printf(" ERROR! %s device not found!!\n",interface);
		return NMTOOL_STATUS_ERROR;
	}
	return NMTOOL_STATUS_SUCCESS;
}

static NMTOOL_STATUS nmtool_interface_up(char *interface)
{
	NMConnection *pcon;
	NMRemoteConnection *prcon;
	NMDevice *pnmdev;
	NMAccessPoint *pnmap;
	NMSettingWireless *pnmswifi;
	GMainLoop *pgm_loop;
	GError *error;
	char *pspec_obj = NULL;
	char *ssid;
	ConnCBdata cbdata;
	NMTOOL_STATUS ret;

	ret = nmtool_get_nmconn_from_interface(interface, &pcon, &prcon);
	if (NMTOOL_STATUS_SUCCESS != ret)
		return ret;

	pnmdev = nm_client_get_device_by_iface(nm_cli.client, interface);
	if (!pnmdev) {
		return NMTOOL_STATUS_ERROR;
	}

	if ((0 == strcmp(interface,"wlan0")) ||
			(0 == strcmp(interface,"uap0")))
	{
		GBytes *pssid_bytes;

		pnmswifi = nm_connection_get_setting_wireless(pcon);
		if (!pnmswifi) {
			return NMTOOL_STATUS_ERROR;
		}

		pssid_bytes = nm_setting_wireless_get_ssid(pnmswifi);
		if (!pssid_bytes) {
			return NMTOOL_STATUS_ERROR;
		}

		ssid = nm_utils_ssid_to_utf8(g_bytes_get_data (pssid_bytes, NULL),
				g_bytes_get_size (pssid_bytes));
		if (!ssid) {
			return NMTOOL_STATUS_ERROR;
		}

		ret = nmtool_get_access_point(interface, pnmdev, ssid, &pnmap);

		if (NMTOOL_STATUS_SUCCESS == ret) {
			pspec_obj = nm_object_get_path(NM_OBJECT (pnmap));
		}

	}
	pgm_loop = g_main_loop_new(NULL, TRUE);
	cbdata.ploop = pgm_loop;
	cbdata.prcon = prcon;

	nm_client_activate_connection_async( nm_cli.client,
			pcon, pnmdev, pspec_obj, NULL,
			connection_activate_cb, (gpointer) &cbdata);
	g_main_loop_run(pgm_loop);
	g_main_loop_unref (pgm_loop);
	return NMTOOL_STATUS_SUCCESS;
}

static NMTOOL_STATUS nmtool_interface_down(char *interface)
{
	NMTOOL_STATUS ret;
	NMConnection *pcon;
	NMRemoteConnection *prcon;
	NMActiveConnection *pnm_acon;
	GError *error = NULL;


	ret = nmtool_get_nmconn_from_interface(interface, &pcon, &prcon);
	if (NMTOOL_STATUS_SUCCESS != ret)
		return ret;

	ret = nmtool_get_active_connection(nm_connection_get_uuid(pcon), &pnm_acon);
	if (NMTOOL_STATUS_SUCCESS != ret)
		return ret;

	if (FALSE != nm_client_deactivate_connection(nm_cli.client, pnm_acon, NULL, &error)) {
		return NMTOOL_STATUS_SUCCESS;
	}
	return NMTOOL_STATUS_ERROR;
}

NMTOOL_STATUS set_interface_status(char *interface, intf_state state)
{
	NMTOOL_STATUS ret;
	if (NULL == interface)
		return NMTOOL_STATUS_ERR_INTERFACE;

	switch (state) {
		case up:
			return nmtool_interface_up(interface);
		case down:
			return nmtool_interface_down(interface);
		default:
			break;
	}

	return NMTOOL_STATUS_ERROR;
}

static void connection_added_cb(GObject *client, GAsyncResult *result, gpointer *pdat)
{
	ConnCBdata *pcbdat = (ConnCBdata *) pdat;
	GError *perr = NULL;

	if (!nm_remote_connection_commit_changes_finish (NM_REMOTE_CONNECTION (pcbdat->prcon), result, &perr)) {
		g_error_free(perr);
	}
	g_main_loop_quit(pcbdat->ploop);
}

static void nmtool_save_remote_connection_info(NMRemoteConnection *prcon)
{
	ConnCBdata cbdata;
	GMainLoop *pgm_loop;

	pgm_loop = g_main_loop_new(NULL, TRUE);

	cbdata.ploop = pgm_loop;
	cbdata.prcon = prcon;

	nm_remote_connection_commit_changes_async (prcon, TRUE,
                                                   NULL, connection_added_cb,
						   (gpointer) &cbdata);

	g_main_loop_run(pgm_loop);
}


static gboolean get_connection_for_intf(char *intf, NMConnection *pconn)
{
	const char *cur_intf;
	cur_intf = nm_connection_get_interface_name(pconn);
	if (!cur_intf)
		return FALSE;

	if (0 == strncmp(cur_intf,intf,strlen(cur_intf)))
		return TRUE;
	else
		return FALSE;
}

static NMTOOL_STATUS nmtool_get_nmconn_from_interface(char *interface, NMConnection **ppcon, NMRemoteConnection **pprcon)
{
	const GPtrArray *connections;
	int ix;

	if (NULL == interface)
		return NMTOOL_STATUS_ERR_INTERFACE;

	connections = nm_client_get_connections (nm_cli.client);
	if (!connections) {
		return NMTOOL_STATUS_ERROR;
	}

	for (ix = 0; ix < connections->len; ix++)
	{
		//*ppcon = (NMConnection *) connections->pdata[ix];
		*pprcon = (NMRemoteConnection *) connections->pdata[ix];
		if (!pprcon) {
			*ppcon = NULL;
			return NMTOOL_STATUS_ERROR;
		}
		*ppcon = &((*pprcon)->parent);
		char *pintf;

		if (FALSE != get_connection_for_intf(interface,*ppcon))
		{
			return NMTOOL_STATUS_SUCCESS;
		}
	}
	return NMTOOL_STATUS_ERROR;
}

NMTOOL_STATUS get_SSID_for_interface(char *interface, nm_ssid *ssid)
{
	const GPtrArray *connections;
	int ix;

	if (NULL == interface)
		return NMTOOL_STATUS_ERR_INTERFACE;

	connections = nm_client_get_connections (nm_cli.client);
	if (!connections) {
		return NMTOOL_STATUS_ERROR;
	}

	for (ix = 0; ix < connections->len; ix++)
	{
		NMConnection *pcon = (NMConnection *) connections->pdata[ix];
		char *pintf;

		if (FALSE != get_connection_for_intf(interface,pcon))
		{
			NMDevice *ndev;
			NMDeviceType dev_type;
			NMSettingWireless *nmWifiSetting;
			GBytes *gb_ssid;
			char *pssid_str;

			ndev = nm_client_get_device_by_iface(nm_cli.client, interface);
			if (!ndev) {
				return NMTOOL_STATUS_ERROR;
			}

			dev_type = nm_device_get_device_type(ndev);
			if (NM_DEVICE_TYPE_WIFI != dev_type)
				return NMTOOL_STATUS_ERROR;

			nmWifiSetting = nm_connection_get_setting_wireless(pcon);
			if (!nmWifiSetting)
				return NMTOOL_STATUS_ERROR;

			gb_ssid = nm_setting_wireless_get_ssid(nmWifiSetting);
			if (!gb_ssid)
				return NMTOOL_STATUS_ERROR;

			pssid_str = nm_utils_ssid_to_utf8 (g_bytes_get_data (gb_ssid, NULL),
					g_bytes_get_size (gb_ssid));
			if (!pssid_str)
				return NMTOOL_STATUS_ERROR;

			strncpy(ssid->ssid, pssid_str, 32);
			ssid->ssid[32] = 0;
			return NMTOOL_STATUS_SUCCESS;
		}
	}
	return NMTOOL_STATUS_ERROR;
}

NMTOOL_STATUS get_dhcpmode_for_interface(char *interface, dhcp_state *state)
{
	const GPtrArray *connections;
	int ix;

	if (NULL == interface)
		return NMTOOL_STATUS_ERR_INTERFACE;

	connections = nm_client_get_connections (nm_cli.client);
	if (!connections) {
		return NMTOOL_STATUS_ERROR;
	}

	for (ix = 0; ix < connections->len; ix++)
	{
		NMConnection *pcon = (NMConnection *) connections->pdata[ix];
		char *pintf;

		if (FALSE != get_connection_for_intf(interface,pcon))
		{
			NMSettingIPConfig *ipcfg;
			char *nm_ip_method = NULL;

			ipcfg = nm_connection_get_setting_ip4_config(pcon);
			if (!ipcfg) {
				return NMTOOL_STATUS_ERROR;
			}

			nm_ip_method = nm_setting_ip_config_get_method(ipcfg);
			if (!nm_ip_method) {
				return NMTOOL_STATUS_ERROR;
			}

			if ( 0 == strcmp(nm_ip_method,"auto") )
				*state = dhcp_on;
			else
				*state = dhcp_off;

			return NMTOOL_STATUS_SUCCESS;
		}
	}
	return NMTOOL_STATUS_ERROR;
}

NMTOOL_STATUS set_dhcpmode_for_interface(char *interface, dhcp_state state)
{
	NMConnection *pcon;
	NMRemoteConnection *prcon;
	NMTOOL_STATUS ret;
	NMSettingIPConfig *pnms_ipcfg;

	ret = nmtool_get_nmconn_from_interface(interface, &pcon, &prcon);
	if (NMTOOL_STATUS_SUCCESS != ret)
		return ret;

	pnms_ipcfg = nm_connection_get_setting_ip4_config(pcon);
	if (!pnms_ipcfg)
		return NMTOOL_STATUS_ERROR;

	if (dhcp_on == state)
		g_object_set(G_OBJECT(pnms_ipcfg), NM_SETTING_IP_CONFIG_METHOD, "auto", NULL);
	else
		g_object_set(G_OBJECT(pnms_ipcfg), NM_SETTING_IP_CONFIG_METHOD, "manual", NULL);

	nmtool_save_remote_connection_info(prcon);

	return NMTOOL_STATUS_SUCCESS;
}

NMTOOL_STATUS get_ip_address_for_interface(char *interface, ip4addr *addr)
{
	NMConnection *pcon;
	NMRemoteConnection *prcon;
	NMTOOL_STATUS ret;
	NMSettingIPConfig *ipcfg;
	NMIPAddress *nmipaddr;
	guint num_addresses = 0;
	const char *ipaddress = NULL;

	ret = nmtool_get_nmconn_from_interface(interface, &pcon, &prcon);
	if (NMTOOL_STATUS_SUCCESS != ret)
		return ret;

	ipcfg = nm_connection_get_setting_ip4_config(pcon);
	if (!ipcfg) {
		return NMTOOL_STATUS_ERROR;
	}

	num_addresses = nm_setting_ip_config_get_num_addresses(ipcfg);
	if (0 == num_addresses) {
		NMDevice *pdevice;
		NMActiveConnection *pnm_acon;
		NMIPConfig *pnm_ip4cfg;
		GPtrArray *pipaddrss;

		ret = nmtool_get_active_connection(nm_connection_get_uuid(pcon), &pnm_acon);
		if (NMTOOL_STATUS_SUCCESS != ret)
			return ret;

		pnm_ip4cfg = nm_active_connection_get_ip4_config(pnm_acon);
		if (!pnm_ip4cfg) {
			return NMTOOL_STATUS_ERROR;
		}

		pipaddrss = nm_ip_config_get_addresses(pnm_ip4cfg);
		if (!pipaddrss) {
			return NMTOOL_STATUS_ERROR;
		}

		if (pipaddrss->len > 0)
		{
			char *szIpBuf;
			szIpBuf = nm_ip_address_get_address((NMIPAddress *) pipaddrss->pdata[0]);
			if (!szIpBuf)
				return NMTOOL_STATUS_ERROR;

			strcpy(addr->ip, szIpBuf);
			return NMTOOL_STATUS_SUCCESS;
		}
		else
			return NMTOOL_STATUS_NO_IPADDRESS;
	}
	else
		nmipaddr = nm_setting_ip_config_get_address(ipcfg,0);

	if (!nmipaddr) {
		return NMTOOL_STATUS_ERROR;
	}

	ipaddress = nm_ip_address_get_address(nmipaddr);
	if (!ipaddress) {
		addr->ip[0] = 0;
		return NMTOOL_STATUS_NO_IPADDRESS;
	}
	strcpy(addr->ip, ipaddress);

	return NMTOOL_STATUS_SUCCESS;
}


NMTOOL_STATUS set_ip_address_for_interface(char *interface, ip4addr *addr)
{
	NMConnection *pcon;
	NMRemoteConnection *prcon;
	NMTOOL_STATUS ret;
	NMSettingIPConfig *ipcfg;
	NMIPAddress *nmipaddr, *newipaddr;
	guint num_addresses = 0;
	const char *ipaddress = NULL;

	ret = nmtool_get_nmconn_from_interface(interface, &pcon, &prcon);
	if (NMTOOL_STATUS_SUCCESS != ret)
		return ret;

	ipcfg = nm_connection_get_setting_ip4_config(pcon);
	if (!ipcfg) {
		return NMTOOL_STATUS_ERROR;
	}

	num_addresses = nm_setting_ip_config_get_num_addresses(ipcfg);
	if (0 == num_addresses) {
		guint prefix = 24;
		GError *error;

		nmipaddr = nm_ip_address_new(AF_INET, addr->ip, prefix, &error);
		if (!nmipaddr)
			return NMTOOL_STATUS_ERROR;

		if (FALSE == nm_setting_ip_config_add_address(ipcfg, nmipaddr)) {
			nm_ip_address_unref(nmipaddr);
			return NMTOOL_STATUS_ERROR;
		}
	}
	else {
		nmipaddr = nm_setting_ip_config_get_address(ipcfg,0);
		if (!nmipaddr) {
			return NMTOOL_STATUS_ERROR;
		}
	}

	nm_ip_address_set_address(nmipaddr, addr->ip);
	nmtool_save_remote_connection_info(prcon);

	return NMTOOL_STATUS_SUCCESS;
}


NMTOOL_STATUS get_subnet_for_interface(char *interface, ip4addr *addr)
{
	NMConnection *pcon;
	NMRemoteConnection *prcon;
	NMTOOL_STATUS ret;
	NMSettingIPConfig *ipcfg;
	NMIPAddress *nmipaddr;
	guint num_addresses = 0;
	guint ip_prefix, n_netmask;

	ret = nmtool_get_nmconn_from_interface(interface, &pcon, &prcon);
	if (NMTOOL_STATUS_SUCCESS != ret)
		return ret;

	ipcfg = nm_connection_get_setting_ip4_config(pcon);
	if (!ipcfg) {
		return NMTOOL_STATUS_ERROR;
	}

	num_addresses = nm_setting_ip_config_get_num_addresses(ipcfg);
	if (0 == num_addresses) {
		NMDevice *pdevice;
		NMActiveConnection *pnm_acon;
		NMIPConfig *pnm_ip4cfg;
		GPtrArray *pipaddrss;

		ret = nmtool_get_active_connection(nm_connection_get_uuid(pcon), &pnm_acon);
		if (NMTOOL_STATUS_SUCCESS != ret)
			return ret;

		pnm_ip4cfg = nm_active_connection_get_ip4_config(pnm_acon);
		if (!pnm_ip4cfg) {
			return NMTOOL_STATUS_ERROR;
		}

		pipaddrss = nm_ip_config_get_addresses(pnm_ip4cfg);
		if (!pipaddrss) {
			return NMTOOL_STATUS_ERROR;
		}

		if (pipaddrss->len > 0)
		{
			nmipaddr = (NMIPAddress *) pipaddrss->pdata[0];
		}
		else
			return NMTOOL_STATUS_NO_IPADDRESS;
	}
	else {
		nmipaddr = nm_setting_ip_config_get_address(ipcfg,0);
		if (!nmipaddr) {
			return NMTOOL_STATUS_NO_IPADDRESS;
		}
	}

	ip_prefix = nm_ip_address_get_prefix(nmipaddr);
	n_netmask = nm_utils_ip4_prefix_to_netmask(ip_prefix);
	nm_utils_inet4_ntop( (in_addr_t) n_netmask, addr->ip);

	return NMTOOL_STATUS_SUCCESS;
}

//TO DO
NMTOOL_STATUS set_subnet_for_interface(char *interface, ip4addr *addr)
{
	NMConnection *pcon;
	NMRemoteConnection *prcon;
	NMTOOL_STATUS ret;
	NMSettingIPConfig *ipcfg;
	NMIPAddress *nmipaddr, *newipaddr;
	guint ip_prefix;
	struct in_addr ipaddr_binary;

	ret = nmtool_get_nmconn_from_interface(interface, &pcon, &prcon);
	if (NMTOOL_STATUS_SUCCESS != ret)
		return ret;

	ipcfg = nm_connection_get_setting_ip4_config(pcon);
	if (!ipcfg) {
		return NMTOOL_STATUS_ERROR;
	}

	nmipaddr = nm_setting_ip_config_get_address(ipcfg,0);
	if (!nmipaddr) {
		return NMTOOL_STATUS_ERROR;
	}

	if (0 == inet_aton(addr->ip, &ipaddr_binary))
		return NMTOOL_STATUS_ERROR;
	ip_prefix = nm_utils_ip4_netmask_to_prefix((guint32) ipaddr_binary.s_addr);
	nm_ip_address_set_prefix(nmipaddr, ip_prefix);
	//printf("%s():%d netmask:%s n_binary:%08x prefix:%d\n",__func__,__LINE__,addr->ip,ipaddr_binary.s_addr,ip_prefix);
	nmtool_save_remote_connection_info(prcon);

	return NMTOOL_STATUS_SUCCESS;
}

static NMTOOL_STATUS __get_gateway_for_interface(char *interface, ip4addr *addr, char *uuid)
{
	NMConnection *pcon;
	NMRemoteConnection *prcon;
	NMSettingIPConfig *ipcfg;
	const char *ipaddress = NULL;

	NMTOOL_STATUS ret;
	GPtrArray *pActiveConns;
	NMActiveConnection *pnm_acon = NULL;
	NMIPConfig *pnm_ipcfg;
	int ix;
	char *cur_uuid;
	char *szgateway = NULL;

	ret  = nmtool_get_active_connection(uuid, &pnm_acon);
	if (NMTOOL_STATUS_SUCCESS != ret)
		return ret;

	pnm_ipcfg = nm_active_connection_get_ip4_config(pnm_acon);
	if (!pnm_ipcfg) {
		//printf("%s(): %d NMIPConfig NOT found!\n",__func__,__LINE__);
		return NMTOOL_STATUS_ERROR;
	}

	szgateway = nm_ip_config_get_gateway(pnm_ipcfg);
	if (!szgateway) {
		//printf("%s(): %d gateway NOT found!\n",__func__,__LINE__);
		return NMTOOL_STATUS_ERROR;
	}

	strcpy(addr->ip, szgateway);

	return NMTOOL_STATUS_SUCCESS;

}

NMTOOL_STATUS get_gateway_for_interface(char *interface, ip4addr *addr)
{
	NMConnection *pcon;
	NMRemoteConnection *prcon;
	NMTOOL_STATUS ret;
	NMSettingIPConfig *ipcfg;
	const char *ipaddress = NULL;
	ret = nmtool_get_nmconn_from_interface(interface, &pcon, &prcon);
	if (NMTOOL_STATUS_SUCCESS != ret)
		return ret;

	ipcfg = nm_connection_get_setting_ip4_config(pcon);
	if (!ipcfg) {
		return NMTOOL_STATUS_ERROR;
	}

	ipaddress = nm_setting_ip_config_get_gateway(ipcfg);
	if (!ipaddress) {
		addr->ip[0] = 0;
		return __get_gateway_for_interface(interface, addr, nm_connection_get_uuid(pcon));
	}
	strcpy(addr->ip, ipaddress);

	return NMTOOL_STATUS_SUCCESS;
}

NMTOOL_STATUS set_gateway_for_interface(char *interface, ip4addr *addr)
{
	NMConnection *pcon;
	NMRemoteConnection *prcon;
	NMTOOL_STATUS ret;
	NMSettingIPConfig *ipcfg;
	NMIPAddress *nmipaddr, *newipaddr;
	const char *ipaddress = NULL;
	guint ip_prefix;
	struct in_addr ipaddr_binary;

	ret = nmtool_get_nmconn_from_interface(interface, &pcon, &prcon);
	if (NMTOOL_STATUS_SUCCESS != ret)
		return ret;

	ipcfg = nm_connection_get_setting_ip4_config(pcon);
	if (!ipcfg) {
		return NMTOOL_STATUS_ERROR;
	}

	g_object_set( G_OBJECT(ipcfg), NM_SETTING_IP_CONFIG_GATEWAY, addr->ip, NULL);

	nmtool_save_remote_connection_info(prcon);

done:
	return NMTOOL_STATUS_SUCCESS;
}

NMTOOL_STATUS nmtool_get_name_servers_for_interface(char *interface, char ***pppNameServers)
{
	NMTOOL_STATUS ret;
	char *psz_dnsmode;
	char *psz_dnsrcmgr;
	GPtrArray *pdnslist;
	NMDnsEntry *pnmdns;
	int ix;


	pdnslist = nm_client_get_dns_configuration(nm_cli.client);
	if (!pdnslist) {
		return NMTOOL_STATUS_ERROR;
	}

	for (ix = 0; ix < pdnslist->len; ix++) {
		char *pcur_intf;
		//char **ppNameServers;
		int iy;
		pnmdns = pdnslist->pdata[ix];
		if (!pnmdns) {
			continue;
		}

		pcur_intf = nm_dns_entry_get_interface(pnmdns);
		if (!pcur_intf) {
			continue;
		}

		if (0 != strcmp(pcur_intf, interface)){
			continue;
		}

		*pppNameServers = nm_dns_entry_get_nameservers(pnmdns);
		if (!*pppNameServers) {
			return NMTOOL_STATUS_ERROR;
		}

		return NMTOOL_STATUS_SUCCESS;
	}
	return NMTOOL_STATUS_ERROR;
}

NMTOOL_STATUS get_dns_for_interface(char *interface, ip4addr *addr)
{
	NMTOOL_STATUS ret;
	char **ppNameServers;
	int ix;

	ret = nmtool_get_name_servers_for_interface(interface, &ppNameServers);
	if (NMTOOL_STATUS_SUCCESS != ret)
		return NMTOOL_STATUS_ERROR;
	//for (ix = 0; ppNameServers[ix]; ix++) {
		//printf("%s():%d NameServer[%d]:%s\n",__func__,__LINE__,ix,ppNameServers[ix]);

	//}
	strcpy(addr->ip, ppNameServers[0]);
	return NMTOOL_STATUS_SUCCESS;
}

NMTOOL_STATUS set_dns_for_interface(char *interface, ip4addr *addr)
{
	NMConnection *pcon;
	NMRemoteConnection *prcon;
	NMTOOL_STATUS ret;
	NMSettingIPConfig *ipcfg;
	NMIPAddress *nmipaddr, *newipaddr;
	const char *ipaddress = NULL;

	ret = nmtool_get_nmconn_from_interface(interface, &pcon, &prcon);
	if (NMTOOL_STATUS_SUCCESS != ret)
		return ret;

	ipcfg = nm_connection_get_setting_ip4_config(pcon);
	if (!ipcfg) {
		return NMTOOL_STATUS_ERROR;
	}

	if (FALSE == nm_setting_ip_config_add_dns(ipcfg, addr->ip))
		return NMTOOL_STATUS_ERROR;
	nmtool_save_remote_connection_info(prcon);

	return NMTOOL_STATUS_SUCCESS;
}


NMTOOL_STATUS get_alternate_dns_for_interface(char *interface, ip4addr *addr)
{
	NMTOOL_STATUS ret;
	char **ppNameServers;
	int ix;

	ret = nmtool_get_name_servers_for_interface(interface, &ppNameServers);
	if (NMTOOL_STATUS_SUCCESS != ret)
		return NMTOOL_STATUS_ERROR;
	for (ix = 0; ppNameServers[ix]; ix++) {
		//printf("%s():%d NameServer[%d]:%s\n",__func__,__LINE__,ix,ppNameServers[ix]);
		if (1 == ix) {
			strcpy(addr->ip, ppNameServers[ix]);
			return NMTOOL_STATUS_SUCCESS;
		}
	}
	return NMTOOL_STATUS_ERROR;
}

NMTOOL_STATUS set_alternate_dns_for_interface(char *interface, ip4addr *addr)
{
	NMConnection *pcon;
	NMRemoteConnection *prcon;
	NMTOOL_STATUS ret;
	NMSettingIPConfig *ipcfg;
	NMIPAddress *nmipaddr, *newipaddr;
	const char *ipaddress = NULL;

	ret = nmtool_get_nmconn_from_interface(interface, &pcon, &prcon);
	if (NMTOOL_STATUS_SUCCESS != ret)
		return ret;

	ipcfg = nm_connection_get_setting_ip4_config(pcon);
	if (!ipcfg) {
		return NMTOOL_STATUS_ERROR;
	}

	if (FALSE == nm_setting_ip_config_add_dns(ipcfg, addr->ip))
		return NMTOOL_STATUS_ERROR;
	nmtool_save_remote_connection_info(prcon);

	return NMTOOL_STATUS_SUCCESS;

}

NMTOOL_STATUS get_mac_address_for_interface(char *interface, macaddr *addr)
{
	const GPtrArray *devices;
	int ix;
	const char *nmintf;
	char *psz_hwaddr;

	devices = nm_client_get_devices(nm_cli.client);

	if (!devices)
	{
		printf("%s():%d Error getting devices!\n",__func__,__LINE__);
		return NMTOOL_STATUS_ERROR;
	}

	for(ix = 0; ix < devices->len; ix++)
	{
		NMDevice *nmd =  (NMDevice *) devices->pdata[ix];
		nmintf = nm_device_get_iface(nmd);
		if (!nmintf)
			continue;
		//printf("%s():%d [%d]:intf:%s \n",__func__,__LINE__,ix,nmintf);
		if (0 != strcmp(interface, nmintf))
			continue;

		psz_hwaddr = nm_device_get_hw_address(nmd);
		if (!psz_hwaddr)
			return NMTOOL_STATUS_ERROR;

		//printf("%s():%d [%d]:MAC:%s \n",__func__,__LINE__,ix,psz_hwaddr);
		strcpy(addr->mac, psz_hwaddr);
		return NMTOOL_STATUS_SUCCESS;
	}

	return NMTOOL_STATUS_ERROR;
}

NMTOOL_STATUS set_mac_address_for_interface(char *interface, macaddr *addr)
{
	NMConnection *pcon;
	NMRemoteConnection *prcon;
	NMTOOL_STATUS ret;

//Setting MAC address to connection does not change device settings.
//Actual mac address setting to be implemented later.
	return NMTOOL_STATUS_ERROR;

	if ( 17 != strnlen(addr->mac,18))
		return NMTOOL_STATUS_ERROR;

	ret = nmtool_get_nmconn_from_interface(interface, &pcon, &prcon);
	if (NMTOOL_STATUS_SUCCESS != ret)
		return ret;

	if ((0 == strcmp(interface,"wlan0")) ||
			(0 == strcmp(interface,"uap0")))
	{
		NMSettingWireless *pnmswifi;

		pnmswifi = nm_connection_get_setting_wireless(pcon);
		if (!pnmswifi) {
			return NMTOOL_STATUS_ERROR;
		}
		g_object_set(G_OBJECT(pnmswifi), NM_SETTING_WIRELESS_MAC_ADDRESS, addr->mac, NULL);
	}
	else if (0 == strcmp(interface,"eth0"))
	{
		NMSettingWired *pnmseth;

		pnmseth = nm_connection_get_setting_wired(pcon);
		if (!pnmseth) {
			return NMTOOL_STATUS_ERROR;
		}
		g_object_set(G_OBJECT(pnmseth), NM_SETTING_WIRED_MAC_ADDRESS, addr->mac,NULL);
	}
	else
		return NMTOOL_STATUS_ERROR;

	nmtool_save_remote_connection_info(prcon);

	return NMTOOL_STATUS_SUCCESS;
}

NMTOOL_STATUS get_wifi_scan_results(char *interface, APList *plist)
{
	NMDeviceType dev_type;
	int ix, list_size;
	char *psz_ssid;

	NMDevice *pnmdev;
	GPtrArray *pAccessPoints;
	NMAccessPoint *pnmap;
	GBytes *pssid_bytes;
	gconstpointer gcp_ssid_dat;
	gsize	gs_ssid_size;

	pnmdev = nm_client_get_device_by_iface(nm_cli.client, interface);
	if (!pnmdev) {
		return NMTOOL_STATUS_ERROR;
	}

	dev_type = nm_device_get_device_type(pnmdev);
	if (NM_DEVICE_TYPE_WIFI != dev_type)
		return NMTOOL_STATUS_ERROR;

	nm_device_wifi_request_scan(pnmdev,NULL,NULL);

	pAccessPoints = nm_device_wifi_get_access_points( NM_DEVICE_WIFI(pnmdev));
	if (!pAccessPoints) {
		return NMTOOL_STATUS_ERROR;
	}
	//printf("%s():%d  DEBUG!! num aps:%d\n",__func__,__LINE__,pAccessPoints->len);

	list_size = plist->num_entries;
	plist->num_entries = 0;
	for (ix = 0; ix < pAccessPoints->len; ix++) {
		//pnmap = (NMAccessPoint *) pAccessPoints->pdat[ix];
		plist->pAPlist[plist->num_entries].ssid[0] = 0;
		pnmap = g_ptr_array_index(pAccessPoints, ix);
		if (!pnmap)
			continue;
		pssid_bytes = nm_access_point_get_ssid(pnmap);
		if (!pssid_bytes)
			continue;

		if (plist->num_entries >= list_size) {
			return NMTOOL_STATUS_INSUFFICIENT_BUFFER;
		}

		psz_ssid = nm_utils_ssid_to_utf8 (g_bytes_get_data (pssid_bytes, NULL),
				g_bytes_get_size (pssid_bytes));
		if (psz_ssid) {
			strcpy(plist->pAPlist[plist->num_entries].ssid,psz_ssid);
			plist->num_entries++;
			//printf("%s():%d  DEBUG!! [%d] ssid_size:%d  ssid:%s_\n",__func__,__LINE__, ix, gs_ssid_size, psz_ssid);
		}
	}

	return NMTOOL_STATUS_SUCCESS;
}

NMTOOL_STATUS nmtool_get_access_point(char *interface, NMDevice *pnmdev, char *ssid, NMAccessPoint **ppnmap)
{
	GPtrArray *pAccessPoints;
	NMAccessPoint *pnmap;
	GBytes *pssid_bytes;
	char *pssid_str;
	int ix;

	pAccessPoints = nm_device_wifi_get_access_points( NM_DEVICE_WIFI(pnmdev));
	if (!pAccessPoints) {
		return NMTOOL_STATUS_ERROR;
	}

	for (ix = 0; ix < pAccessPoints->len; ix++) {
		pnmap = g_ptr_array_index(pAccessPoints, ix);
		if (!pnmap)
			continue;
		pssid_bytes = nm_access_point_get_ssid(pnmap);
		if (!pssid_bytes)
			continue;

		pssid_str = nm_utils_ssid_to_utf8 (g_bytes_get_data (pssid_bytes, NULL),
					g_bytes_get_size (pssid_bytes));
		if (0 == strcmp(pssid_str, ssid)) {
			*ppnmap = pnmap;
			return NMTOOL_STATUS_SUCCESS;
		}
	}
	return NMTOOL_STATUS_ERROR;
}

NMTOOL_STATUS connect_to_access_point(char *interface, char *ssid, char *psk)
{
	NMConnection *pcon;
	NMDevice *pnmdev;
	NMAccessPoint *pnmap;
	NMRemoteConnection *prcon;
	NMSettingConnection *pnmscon;
	NMSettingWireless *pnms_wireless;
	NMSettingWirelessSecurity *pnms_wireless_sec;
	NMSettingIP4Config *pnms_ip4cfg;
	NMTOOL_STATUS ret;
	ConnCBdata cbdata;
	GMainLoop *pgm_loop;
	gboolean bNewConnection = FALSE;
	GBytes *ssid_bytes;
	char *pspec_obj = NULL;

	ret = nmtool_get_nmconn_from_interface(interface, &pcon, &prcon);
	if (NMTOOL_STATUS_SUCCESS != ret) {
		//There is no saved connection.  Creating a new one.
		char *uuid;

		pcon = nm_simple_connection_new();
		if (!pcon)
			return NMTOOL_STATUS_ERROR;

		pnmscon = (NMSettingConnection *) nm_setting_connection_new();
		if (!pnmscon)
			return NMTOOL_STATUS_ERROR;

		uuid = nm_utils_uuid_generate();
		g_object_set(G_OBJECT(pnmscon), NM_SETTING_CONNECTION_UUID, uuid,
				NM_SETTING_CONNECTION_ID, "wifi",
				NM_SETTING_CONNECTION_TYPE, "802-11-wireless",
				NM_SETTING_CONNECTION_AUTOCONNECT, TRUE,
				NM_SETTING_CONNECTION_INTERFACE_NAME, interface,
				NULL);
		g_free(uuid);
		nm_connection_add_setting(pcon, NM_SETTING(pnmscon));

		pnms_wireless = (NMSettingWireless*) nm_setting_wireless_new();
		if (!pnms_wireless)
			return NMTOOL_STATUS_ERROR;

		pnms_wireless_sec = (NMSettingWirelessSecurity *) nm_setting_wireless_security_new();
		if (!pnms_wireless_sec)
			return NMTOOL_STATUS_ERROR;

		pnms_ip4cfg = (NMSettingIP4Config *) nm_setting_ip4_config_new();
		if (!pnms_ip4cfg)
			return NMTOOL_STATUS_ERROR;

		g_object_set(G_OBJECT(pnms_ip4cfg), NM_SETTING_IP_CONFIG_METHOD, "auto", NULL);

		nm_connection_add_setting(pcon, NM_SETTING(pnms_wireless));
		nm_connection_add_setting(pcon, NM_SETTING(pnms_wireless_sec));
		nm_connection_add_setting(pcon, NM_SETTING(pnms_ip4cfg));
		bNewConnection = TRUE;
	}

	pnms_wireless = nm_connection_get_setting_wireless(pcon);
	pnms_wireless_sec = nm_connection_get_setting_wireless_security(pcon);

	if ( (NULL == pnms_wireless) ||
	     (NULL == pnms_wireless_sec)) {
		g_object_unref(pcon);
		return NMTOOL_STATUS_ERROR;
	}

	if (32 < strnlen(ssid,33)) {
		g_object_unref(pcon);
		return NMTOOL_STATUS_INVALID_PARAM;
	}

	ssid_bytes = g_bytes_new (ssid, strlen (ssid));
	g_object_set(G_OBJECT(pnms_wireless), NM_SETTING_WIRELESS_SSID, ssid_bytes,
			NM_SETTING_WIRELESS_MODE, "infrastructure", NULL);

	g_object_set(G_OBJECT(pnms_wireless_sec), NM_SETTING_WIRELESS_SECURITY_KEY_MGMT, "wpa-psk",
			NM_SETTING_WIRELESS_SECURITY_PSK, psk, NULL);

	pnmdev = nm_client_get_device_by_iface(nm_cli.client, interface);
	if (!pnmdev) {
		g_object_unref(pcon);
		return NMTOOL_STATUS_ERROR;
	}

	ret = nmtool_get_access_point("wlan0", pnmdev, ssid, &pnmap);
	if (NMTOOL_STATUS_SUCCESS == ret) {
		pspec_obj = nm_object_get_path(NM_OBJECT (pnmap));
	}

	pgm_loop = g_main_loop_new(NULL, TRUE);
	cbdata.ploop = pgm_loop;
	cbdata.prcon = prcon;

	if (bNewConnection) {
		nm_client_add_and_activate_connection_async( nm_cli.client,
				pcon, pnmdev, pspec_obj, NULL, connection_add_and_activate_cb, (gpointer) &cbdata);
		g_main_loop_run(pgm_loop);
	}
	else {
		nmtool_save_remote_connection_info(prcon);
		nm_client_activate_connection_async( nm_cli.client,
				pcon, pnmdev, pspec_obj, NULL, connection_activate_cb, (gpointer) &cbdata);
		g_main_loop_run(pgm_loop);
	}

	g_main_loop_unref (pgm_loop);

	if (bNewConnection) {
		g_object_unref(pcon);
	}
	return NMTOOL_STATUS_SUCCESS;
}

