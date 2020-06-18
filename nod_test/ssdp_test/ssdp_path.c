diff --git a/server/CMakeLists.txt b/server/CMakeLists.txt
old mode 100644
new mode 100755
index d6b9540..6655cfc
--- a/server/CMakeLists.txt
+++ b/server/CMakeLists.txt
@@ -2,6 +2,9 @@ cmake_minimum_required(VERSION 2.8)
 project(rest_sever)
 set(CMAKE_MACOSX_RPATH 0)
 
+find_package(Threads REQUIRED)
+# target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})
+
 list(APPEND CMAKE_MODULE_PATH cmake)
 
 #libavs_auth
@@ -30,6 +33,11 @@ include_directories(network)
 include_directories(ssdp)
 include_directories(external)
 add_executable(rest_server ${rest_server_srcs})
-target_link_libraries(rest_server avs_auth network ssdp mongoose nmtool)
+target_link_libraries(rest_server avs_auth network ssdp mongoose nmtool Threads::Threads)
 install(TARGETS rest_server DESTINATION /usr/bin)
 install(PROGRAMS rest-server DESTINATION /etc/init.d)
+
+#restclient
+set(server_src server/client.c)
+add_executable(client ${server_src})
+install(TARGETS client DESTINATION /usr/bin)
diff --git a/server/Makefile b/server/Makefile
old mode 100644
new mode 100755
diff --git a/server/network/network.c b/server/network/network.c
old mode 100644
new mode 100755
index 5e8c09c..c1c1654
--- a/server/network/network.c
+++ b/server/network/network.c
@@ -26,6 +26,7 @@ char* get_macaddr(char *interface) {
         free(mac_addr);
         return NULL;
     }
+	printf("===rock=== %s interface=%s mac_addr=%s addr.mac=%s \n",__func__,interface,mac_addr,addr.mac);
     return mac_addr;
 }
 
@@ -50,6 +51,7 @@ char* get_current_ssid(char *interface) {
         free(ssid);
         return NULL;
     }
+	printf("===rock=== %s interface=%s ssid=%s s_ssid.ssid=%s \n",__func__,interface,ssid,s_ssid.ssid);
     return ssid;
 }
 
@@ -81,7 +83,7 @@ char* get_ap_list(char *interface) {
     nret = get_wifi_scan_results(interface, &aplist);
     if ((NMTOOL_STATUS_SUCCESS == nret) || (NMTOOL_STATUS_INSUFFICIENT_BUFFER == nret)) {
         for (int i = 0; i < aplist.num_entries; i++) {
-            printf("Get ssid: %s\n", aplist.pAPlist[i].ssid);
+            printf("===rock=== Get ssid: %s\n", aplist.pAPlist[i].ssid);
             current_size = current_size + new_size;
             ap_list = (char *) realloc(ap_list, current_size);
             strcat(ap_list, "\"");
@@ -106,7 +108,7 @@ char* get_ap_list(char *interface) {
 int connect_wifi(char *interface, char* ssid, char* password)
 {
     NMTOOL_STATUS nret;
-    printf("In connect_wifi function ssid is %s\n", ssid);
+    printf("===rock=== In connect_wifi function ssid =%s password=%s\n", ssid,password);
     nmtool_init();
     nret = connect_to_access_point(interface, ssid, password);
     nmtool_exit();
@@ -151,6 +153,7 @@ void down_hotspot()
     char down_hotspot_cmd[23] = "nmcli con down hotspot";
     puts(&down_hotspot_cmd);
     system(down_hotspot_cmd);
+	printf(" ===rock===  %s cmd=%s \n",__func__,down_hotspot_cmd);
 }
 
 void up_hotspot()
@@ -158,6 +161,7 @@ void up_hotspot()
     char up_hotspot_cmd[21] = "nmcli con up hotspot";
     puts(&up_hotspot_cmd);
     system(up_hotspot_cmd);
+	printf(" ===rock===  %s  cmd=%s \n",__func__,up_hotspot_cmd);
 }
 
 int test_connection()
@@ -169,7 +173,7 @@ int test_connection()
 
   hostname = "amazon.com";
   while (tried < 20) {
-    printf("Test connection %d times\n", tried);
+    printf("===rock=== Test connection %d times\n", tried);
     hostinfo = gethostbyname(hostname);
     if (hostinfo != NULL) {
       res = 0;
diff --git a/server/server/server.c b/server/server/server.c
old mode 100644
new mode 100755
index ad4bd12..5a0eef4
--- a/server/server/server.c
+++ b/server/server/server.c
@@ -4,6 +4,20 @@
 #include "network.h"
 #include "ssdp.h"
 
+#include <stdio.h>
+#include <stdlib.h>
+#include <string.h>
+#include <ctype.h>
+#include <sys/types.h>
+#include <sys/ipc.h>
+#include <sys/socket.h>
+#include <unistd.h>
+#include <netinet/in.h>
+#include <arpa/inet.h>
+#include <errno.h>
+#include <fcntl.h>
+#include <pthread.h>
+#include <unistd.h>
 static const char *s_http_port = "8000";
 static const char *s_eth_ifname = "eth0";
 static const char *s_wifi_ifname = "wlan0";
@@ -58,7 +72,7 @@ char* get_param(struct mg_str *body, char* param_name) {
     for ( i = 1; i < r; i ++ ) {
         if (jsoneq(body->p, &t[i], param_name) == 0) {
             ret = strndup(body->p + t[i+1].start, t[i+1].end-t[i+1].start);
-            printf("Get %s, the value is %s\n", param_name, ret);
+            printf("===rock=== Get %s, the value is %s\n", param_name, ret);
             break;
         }
     }
@@ -73,6 +87,7 @@ char* get_menu_string(const char *menu[], size_t count) {
     for (i = 0; i < count; i++) {
         current_size = current_size + 30;
         //current_size = current_size + sizeof(menu[i]);
+		printf("===rock=== %s menu[%d]=%s\n",__func__,i,menu[i]);
         ret  = (char *) realloc(ret, current_size);
         strcat(ret, "\"");
         strcat(ret, menu[i]);
@@ -139,6 +154,7 @@ static void handle_network_access_point(struct mg_connection *nc, struct http_me
     /* Compute the result and send it back as a JSON object */
     mg_printf_http_chunk(nc, "{ \"result\": %d }", result);
     mg_send_http_chunk(nc, "", 0);
+	printf("===rock=== %s ssid=%s password=%s\n",__func__,ssid,password);
 }
 
 static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
@@ -206,6 +222,108 @@ static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
   }
 }
 
+/**********************************/
+#define SIZE 512
+#define PORT 8880
+
+int sock_bind(int lisfd, int port)
+{
+	printf("===rock=== sock_bind   \n");
+	struct sockaddr_in myaddr;
+	memset((char *)&myaddr, 0, sizeof(struct sockaddr_in));//清零
+	myaddr.sin_family = AF_INET;//IPV4
+	myaddr.sin_port = htons(port);//端口
+	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);//允许连接到所有本地地址上
+	if (bind(lisfd, (struct sockaddr *)&myaddr, sizeof(struct sockaddr))==-1)
+	{
+		perror("sock_bind failed!\n");
+		exit(1);
+	}
+	return 0;
+}
+ 
+void *upd_recvfrom()
+{
+	int lisfd;
+	int i;
+	int flag;
+	int nbytes;
+	socklen_t len;
+	char msg[SIZE];
+	struct sockaddr_in cliaddr;
+	
+	printf("===rock=== 111   \n");	
+	bzero(msg, SIZE);
+	printf("===rock=== 222   \n");
+	len = sizeof(cliaddr);
+	bzero(&cliaddr, sizeof(cliaddr));
+	printf("===rock=== 333   \n");
+	lisfd = socket(AF_INET, SOCK_DGRAM, 0);
+	if (lisfd == -1)
+	{
+		printf("socket failed.\n");
+		exit(1); ;
+	}
+	printf("===rock=== 444   \n");
+
+	struct timeval timeout;
+    timeout.tv_sec = 1;//秒
+    timeout.tv_usec = 0;//微秒
+    if (setsockopt(lisfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
+        perror("setsockopt failed:");
+    }
+
+	sock_bind(lisfd, PORT);
+	printf("===rock=== fcntl   \n");
+	flag = fcntl(lisfd, F_GETFL, 0);
+	if (flag < 0)
+	{
+		printf("fcntl failed.\n");
+		exit(1); ;
+	}
+	flag |= O_NONBLOCK;
+	printf("===rock === 000 fcntl   \n");
+	if (fcntl(lisfd, F_SETFL, flag) < 0)
+	{
+		printf("fcntl failed.\n");
+		exit(1); ;
+	}
+
+	while(1)
+	{
+		usleep(5000);
+		nbytes = recvfrom(lisfd, msg, SIZE, 0, (struct sockaddr *)&cliaddr, &len);
+		if (nbytes == -1 && errno != EAGAIN)
+		{
+			printf("recv failed.\n");
+			return NULL;
+		}else if (nbytes == 0 || (nbytes==-1 && errno==EAGAIN))
+		{
+			continue;
+		}else
+		{
+			printf("recv: %s\n", msg);
+		}
+		
+		for (i=0; i<strlen(msg); i++)//接收到的字符转换为大写的回送给客户端
+		{
+			msg[i] = toupper(msg[i]);
+		}
+		printf("===rock=== sendto:%s \n",msg);
+		nbytes = sendto(lisfd, msg, sizeof(msg), 0, (struct sockaddr *)&cliaddr, len);//udp发送要指定IP和端口
+
+		if (nbytes == -1 && errno != EAGAIN)
+		{
+			printf("recv failed.\n");
+			exit(1) ;
+		}
+	}
+	close(lisfd);
+	return NULL;
+}
+/*********************************/
+
+
 int main(int argc, char *argv[]) {
   struct mg_mgr mgr;
   struct mg_connection *nc;
@@ -216,6 +334,15 @@ int main(int argc, char *argv[]) {
 
   mg_mgr_init(&mgr, NULL);
 
+  pthread_t upd_thread;
+	  printf("===rock=== upd_thread Starting  \n");
+  if (pthread_create(&upd_thread,NULL,upd_recvfrom,NULL)==0)
+	printf("pthread_create ok !\n");
+	
+	
+	printf("===rock=== upd_thread Starting  \n");
+
+
   /* Use current binary directory as document root */
   if (argc > 0 && ((cp = strrchr(argv[0], DIRSEP)) != NULL)) {
     *cp = '\0';
@@ -249,7 +376,7 @@ int main(int argc, char *argv[]) {
   if (access(wifi_config_file, F_OK) != -1) {
     int retry = 1;
     while (retry < 16) {
-      printf("Try to get IP %d: ", retry);
+      printf("Try to get IP %d: \n", retry);
       int getIPres = get_ip(s_wifi_ifname);
       if (getIPres == 0)
         break;
@@ -274,12 +401,13 @@ int main(int argc, char *argv[]) {
   mg_set_protocol_http_websocket(nc);
   s_http_server_opts.enable_directory_listing = "yes";
 
-  printf("Starting rest server on port %s, serving %s\n", s_http_port,
+  printf("===rock=== Starting rest server on port %s, serving %s\n", s_http_port,
          s_http_server_opts.document_root);
   long long last_time = 0;
   for (;;) {
     mg_mgr_poll(&mgr, 1000);
-    last_time = ssdp_send_msearch(last_time, 5000);
+    last_time = ssdp_send_msearch(last_time, 5000);//UDP 发送数据
+	//upd_recvfrom();
   }
   mg_mgr_free(&mgr);
 
diff --git a/server/ssdp/ssdp.c b/server/ssdp/ssdp.c
old mode 100644
new mode 100755
index bd0451b..135191e
--- a/server/ssdp/ssdp.c
+++ b/server/ssdp/ssdp.c
@@ -7,10 +7,41 @@
 #include <net/if.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
+#include <stdlib.h>
+#include<unistd.h>
+#include<sys/types.h>
+#include<sys/stat.h>
+#include<fcntl.h>
+
 #include "ssdp.h"
 
 #define SSDP_BUFFER_LEN 2048
 
+char  buffer[120];
+char  recv_from[120];
+char code_null[] ="null";
+
+
+char  *get_code(){	
+	int fd,size;
+    fd=open("/data/avs/code.cfg",O_RDONLY);
+	if(fd >0)
+	{
+		size=read(fd,buffer,sizeof(buffer));
+		close(fd);
+		printf("===rock=== code=%s \n",buffer);
+		if(strlen(buffer)<2)
+			return code_null;
+		else
+		{
+			//remove("/data/avs/code.cfg");
+			return buffer;
+		}
+	}else{
+		return code_null;
+	}
+}
+
 long long get_current_time() {
     struct timeval time;
     if (gettimeofday(&time, NULL) == -1) {
@@ -20,6 +51,7 @@ long long get_current_time() {
     return (long long) time.tv_sec * 1000 + (long long) time.tv_usec / 1000;
 }
 
+
 int send_msearch() {
     int result = -1;
     char data[SSDP_BUFFER_LEN] = {};
@@ -29,9 +61,10 @@ int send_msearch() {
         "MAN:\"ssdp:discover\"\r\n"
         "MX:1\r\n"
         "ST:urn:%s\r\n"
+		"Code:%s\r\n"
         "\r\n",
         SSDP_ADDR, SSDP_PORT,
-        DEVICE_NAME);
+        DEVICE_NAME,get_code());
 
     size_t data_len = strlen(data);
 
@@ -59,11 +92,22 @@ int send_msearch() {
         goto end;
     }
 
+
     // 4. send data
     if (sendto(fd, data, data_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1) {
         printf("send data failed, errno = %s (%d)\n", strerror(errno), errno);
         goto end;
     }
+/*
+len = recvfrom(socket, buffer, sizeof(buffer), 0, &addr &addr_len);
+
+
+    sendto(sockfd, buffer, len, 0, &addr, addr_len);
+*/
+	//int len = recvfrom(fd, recv_from, sizeof(recv_from), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
+	//printf("=== rock=== recvfrom data   recv_from= %s \n", recv_from);
+
+
 
     result = 0;
 
