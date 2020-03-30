#include <string.h>
#include "mongoose.h"
#include "jsmn.h"
#include "network.h"
#include "ssdp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
static const char *s_http_port = "8000";
static const char *s_eth_ifname = "eth0";
static const char *s_wifi_ifname = "wlan0";
static const char *s_restserver = "It is me, restserver";
static const char *network_menu[] = {"connection_type",
                                     "connection_status",
                                     "current_ssid_name",
                                     "dhcp_mode",
                                     "ip_addresse",
                                     "subnet_mask",
                                     "default_gateway",
                                     "ethernet_mac_address",
                                     "wireless_mac_address",
                                     "wireless_access_points",
                                     "manual_setup",
                                     "test_connection",
                                     "test_connection_results",
                                     "retest_connection"};
static const char *network_manual_setup_menu[] = {"dhcp",
                                                  "ip_address",
                                                  "subnet_mask",
                                                  "default_gateway",
                                                  "pref_dns_server",
                                                  "alt_dns_server"};
static struct mg_serve_http_opts s_http_server_opts;
static const char *wifi_config_file = "/data/system-connections/wifi.nmconnection";

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

char* get_param(struct mg_str *body, char* param_name) {
    int r, i;
    jsmn_parser p;
    jsmntok_t t[128]; /* We expect no more than 128 tokens */
    char *ret = NULL;
    if (body == NULL
        || body->p == NULL
        || body->len <= 0)
        return ret;

    jsmn_init(&p);
    r = jsmn_parse(&p, body->p, body->len, t, sizeof(t)/sizeof(t[0]));
    if (r < 0)  {
                printf("Failed to parse JSON: %d\n", r);
        return ret;
    }
    for ( i = 1; i < r; i ++ ) {
        if (jsoneq(body->p, &t[i], param_name) == 0) {
            ret = strndup(body->p + t[i+1].start, t[i+1].end-t[i+1].start);
            printf("===rock=== Get %s, the value is %s\n", param_name, ret);
            break;
        }
    }
    return ret;
}

char* get_menu_string(const char *menu[], size_t count) {
    size_t current_size = 1;
    char *ret = malloc(current_size);
    *ret = '\0';
    int i = 0;
    for (i = 0; i < count; i++) {
        current_size = current_size + 30;
        //current_size = current_size + sizeof(menu[i]);
		printf("===rock=== %s menu[%d]=%s\n",__func__,i,menu[i]);
        ret  = (char *) realloc(ret, current_size);
        strcat(ret, "\"");
        strcat(ret, menu[i]);
        if (i < count - 1)
            strcat(ret, "\", ");
    }
    return ret;
}

void set_http_headers(struct mg_connection *nc) {
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
}

static void its_restserver(struct mg_connection *nc, struct http_message *hm) {
    set_http_headers(nc);
    mg_printf_http_chunk(nc, s_restserver);
    mg_send_http_chunk(nc, "", 0);
}

static void handle_menu_request(struct mg_connection *nc, struct http_message *hm, const char* menu[], size_t count) {
    set_http_headers(nc);
    char *ret = get_menu_string(menu, count);
    mg_printf_http_chunk(nc, "{ \"Avaiable Menu List\": [ %s ] }", ret);
    mg_send_http_chunk(nc, "", 0);
    free(ret);
}

static void handle_macaddr_request(struct mg_connection *nc, struct http_message *hm, char* ifname) {
    set_http_headers(nc);
    char *ret = "mac 11112233";
    mg_printf_http_chunk(nc, "{ \"MAC_ADDR\": \"%s\" }", ret);
    mg_send_http_chunk(nc, "", 0);
    free(ret);
}

static void handle_current_ssid(struct mg_connection *nc, struct http_message *hm, char* ifname) {
    set_http_headers(nc);
    char *ret = get_current_ssid(ifname);
    mg_printf_http_chunk(nc, "{ \"CURRENT_SSID\": \"%s\" }", ret);
    mg_send_http_chunk(nc, "", 0);
}

static void handle_network_access_points(struct mg_connection *nc, struct http_message *hm, char* ifname) {
    set_http_headers(nc);
    char *ap_list = get_ap_list(ifname);
    mg_printf_http_chunk(nc, "{ \"APList\": [ %s ] }", ap_list);
    mg_send_http_chunk(nc, "", 0);
    free(ap_list);
}

static void handle_network_access_point(struct mg_connection *nc, struct http_message *hm, char* ifname) {
    int result = -1;

    /* Get form variables */
    char* ssid = get_param(&(hm->body), "ssid");
    char* password = get_param(&(hm->body), "password");
    /*if (ssid != NULL) {
        result = connect_wifi(ifname, ssid, password);
    }*/
    free(ssid);
    free(password);

    set_http_headers(nc);
    /* Compute the result and send it back as a JSON object */
    mg_printf_http_chunk(nc, "{ \"result\": %d }", result);
    mg_send_http_chunk(nc, "", 0);
	printf("===rock=== %s ssid=%s password=%s\n",__func__,ssid,password);
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct http_message *hm = (struct http_message *) ev_data;
	
	printf("===rock===  ev_handler\n");

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      if (mg_vcmp(&hm->uri, "/") == 0) {
        its_restserver(nc, hm);
      } else if (mg_vcmp(&hm->uri, "/settings/network") == 0) {
        size_t count = sizeof(network_menu)/sizeof(network_menu[0]);
        handle_menu_request(nc, hm, network_menu, count);
      //TODO
      } else if (mg_vcmp(&hm->uri, "/settings/network/connection_type") == 0) {
        its_restserver(nc, hm);
      } else if (mg_vcmp(&hm->uri, "/settings/network/connection_status") == 0) {
        its_restserver(nc, hm);
      } else if (mg_vcmp(&hm->uri, "/settings/network/current_ssid_name") == 0) {
        handle_current_ssid(nc, hm, s_wifi_ifname);
      //TODO
      } else if (mg_vcmp(&hm->uri, "/settings/network/dhcp_mode") == 0) {
        its_restserver(nc, hm);
      } else if (mg_vcmp(&hm->uri, "/settings/network/ip_address") == 0) {
        its_restserver(nc, hm);
      } else if (mg_vcmp(&hm->uri, "/settings/network/subnet_mask") == 0) {
        its_restserver(nc, hm);
      } else if (mg_vcmp(&hm->uri, "/settings/network/default_gateway") == 0) {
        its_restserver(nc, hm);
      } else if (mg_vcmp(&hm->uri, "/settings/network/ethernet_mac_address") == 0) {
        handle_macaddr_request(nc, hm, s_eth_ifname);
      } else if (mg_vcmp(&hm->uri, "/settings/network/wireless_mac_address") == 0) {
        handle_macaddr_request(nc, hm, s_wifi_ifname);
      } else if (mg_vcmp(&hm->uri, "/settings/network/wireless_access_points") == 0) {
        handle_network_access_points(nc, hm, s_wifi_ifname);
      } else if (mg_vcmp(&hm->uri, "/settings/network/current_access_point") == 0) {
        handle_network_access_point(nc, hm, s_wifi_ifname);
      } else if (mg_vcmp(&hm->uri, "/settings/network/manual_setup") == 0) {
        size_t count = sizeof(network_manual_setup_menu)/sizeof(network_manual_setup_menu[0]);
        handle_menu_request(nc, hm, network_manual_setup_menu, count);
      //TODO
      } else if (mg_vcmp(&hm->uri, "/settings/network/manual_setup/dhcp") == 0) {
        its_restserver(nc, hm);
      } else if (mg_vcmp(&hm->uri, "/settings/network/manual_setup/ip_address") == 0) {
        its_restserver(nc, hm);
      } else if (mg_vcmp(&hm->uri, "/settings/network/manual_setup/subnet_mask") == 0) {
        its_restserver(nc, hm);
      } else if (mg_vcmp(&hm->uri, "/settings/network/manual_setup/default_gateway") == 0) {
        its_restserver(nc, hm);
      } else if (mg_vcmp(&hm->uri, "/settings/network/manual_setup/pref_dns_server") == 0) {
        its_restserver(nc, hm);
      } else if (mg_vcmp(&hm->uri, "/settings/network/manual_setup/alt_dns_server") == 0) {
        its_restserver(nc, hm);
      } else if (mg_vcmp(&hm->uri, "/settings/network/test_connection") == 0) {
        its_restserver(nc, hm);
      } else if (mg_vcmp(&hm->uri, "/settings/network/test_connection/test_connection_results") == 0) {
        its_restserver(nc, hm);
      } else if (mg_vcmp(&hm->uri, "/settings/network/test_connection/retest") == 0) {
        its_restserver(nc, hm);
      } else {
        mg_serve_http(nc, hm, s_http_server_opts); /* Serve static content */
      }
      break;
    default:
      break;
  }
}

/**********************************/
#define SIZE 512
#define PORT 8880

int sock_bind(int lisfd, int port)
{
	printf("===rock=== sock_bind   \n");
	struct sockaddr_in myaddr;
	memset((char *)&myaddr, 0, sizeof(struct sockaddr_in));//清零
	myaddr.sin_family = AF_INET;//IPV4
	myaddr.sin_port = htons(port);//端口
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);//允许连接到所有本地地址上
	if (bind(lisfd, (struct sockaddr *)&myaddr, sizeof(struct sockaddr))==-1)
	{
		perror("sock_bind failed!\n");
		exit(1);
	}
	return 0;
}
 
void *upd_recvfrom()
{
	int lisfd;
	int i;
	int flag;
	int nbytes;
	socklen_t len;
	char msg[SIZE];
	struct sockaddr_in cliaddr;
	
	bzero(msg, SIZE);
	len = sizeof(cliaddr);
	bzero(&cliaddr, sizeof(cliaddr));
	lisfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (lisfd == -1)
	{
		printf("socket failed.\n");
		exit(1); ;
	}
	printf("===rock=== UDP线程收发信息   \n");

	struct timeval timeout;
    timeout.tv_sec = 1;//秒
    timeout.tv_usec = 0;//微秒
    if (setsockopt(lisfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        perror("setsockopt failed:");
    }

	sock_bind(lisfd, PORT);
	flag = fcntl(lisfd, F_GETFL, 0);
	if (flag < 0)
	{
		printf("fcntl failed.\n");
		exit(1); ;
	}
	flag |= O_NONBLOCK;
	if (fcntl(lisfd, F_SETFL, flag) < 0)
	{
		printf("fcntl failed.\n");
		exit(1); ;
	}

	while(1)
	{
		usleep(5000);
		nbytes = recvfrom(lisfd, msg, SIZE, 0, (struct sockaddr *)&cliaddr, &len);
		if (nbytes == -1 && errno != EAGAIN)
		{
			printf("recv failed.\n");
			return NULL;
		}else if (nbytes == 0 || (nbytes==-1 && errno==EAGAIN))
		{
			continue;
		}else
		{
			printf("recv: %s\n", msg);
		}
		
		for (i=0; i<strlen(msg); i++)//接收到的字符转换为大写的回送给客户端
		{
			msg[i] = toupper(msg[i]);
		}
		printf("===rock=== sendto:%s \n",msg);
		nbytes = sendto(lisfd, msg, sizeof(msg), 0, (struct sockaddr *)&cliaddr, len);//udp发送要指定IP和端口

		if (nbytes == -1 && errno != EAGAIN)
		{
			printf("recv failed.\n");
			exit(1) ;
		}
	}
	close(lisfd);
	return NULL;
}
/*********************************/


int main(int argc, char *argv[]) {
  struct mg_mgr mgr;
  struct mg_connection *nc;
  struct mg_bind_opts bind_opts;
  int i;
  char *cp;
  const char *err_str;

  mg_mgr_init(&mgr, NULL);//启动mongoose

  pthread_t upd_thread;
/*
  if (pthread_create(&upd_thread,NULL,upd_recvfrom,NULL)==0)
	{
	printf("pthread_create ok !\n");
	}	
	
	printf("===rock=== UDP线程启动 Starting  \n");

*/
  /* Use current binary directory as document root */
  if (argc > 0 && ((cp = strrchr(argv[0], DIRSEP)) != NULL)) {
    *cp = '\0';
    s_http_server_opts.document_root = argv[0];
  }

  /* Process command line options to customize HTTP server */
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-D") == 0 && i + 1 < argc) {
	  printf("hexdump_file argv= &s \n",argv[+i]);
      mgr.hexdump_file = argv[++i];
    } else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
		printf("document_root argv= &s \n",argv[+i]);
      s_http_server_opts.document_root = argv[++i];
    } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
		printf("s_http_port argv= &s \n",argv[+i]);
      s_http_port = argv[++i];
    } else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
		printf("auth_domain argv= &s \n",argv[+i]);
      s_http_server_opts.auth_domain = argv[++i];
    } else if (strcmp(argv[i], "-P") == 0 && i + 1 < argc) {
		printf("global_auth_file argv= &s \n",argv[+i]);
      s_http_server_opts.global_auth_file = argv[++i];
    } else if (strcmp(argv[i], "-A") == 0 && i + 1 < argc) {
		printf("per_directory_auth_file argv= &s \n",argv[+i]);
      s_http_server_opts.per_directory_auth_file = argv[++i];
    } else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
		printf("url_rewrites argv= &s \n",argv[+i]);
      s_http_server_opts.url_rewrites = argv[++i];
    } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
		printf("s_wifi_ifname argv= &s \n",argv[+i]);
      s_wifi_ifname = argv[++i];
    } else {
      fprintf(stderr, "Unknown option: [%s]\n", argv[i]);
      exit(1);
    }
  }

  if (access(wifi_config_file, F_OK) != -1) {
    int retry = 1;
    while (retry < 16) {
      printf("Try to get IP %d: \n", retry);
      int getIPres = get_ip(s_wifi_ifname);
      if (getIPres == 0)
        break;
      sleep(2);
      retry++;
    }
  }
  //设置HTTP服务器选项 
  memset(&bind_opts, 0, sizeof(bind_opts));

  bind_opts.error_string = &err_str;
  nc = mg_bind_opt(&mgr, s_http_port, ev_handler, bind_opts);
  if (nc == NULL) {
    printf(stderr, "Error starting server on port %s: %s\n", s_http_port,
            *bind_opts.error_string);
    exit(1);
  }

  mg_set_protocol_http_websocket(nc);
  s_http_server_opts.enable_directory_listing = "yes";

  printf("===rock=== Starting rest server on port %s, serving %s\n", s_http_port,
         s_http_server_opts.document_root);
  long long last_time = 0;
  for (;;) {
    mg_mgr_poll(&mgr, 1000);
    last_time = ssdp_send_msearch(last_time, 5000);//UDP 发送数据
	//upd_recvfrom();
  }
  mg_mgr_free(&mgr);

  return 0;
}
