/*************************************************************************
	> File Name: wifi_interface.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2017年12月04日 星期一 11时43分29秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#define WLAN_0  "wlan0"

int main()
{
int i=0;
int sockfd;
struct ifconf ifconf;
char buf[512];
struct ifreq *ifreq;

//初始化ifconf
ifconf.ifc_len = 512;
ifconf.ifc_buf = buf;

if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0)
{
perror("socket");
exit(1);
}  
ioctl(sockfd, SIOCGIFCONF, &ifconf);    //获取所有接口信息

//接下来一个一个的获取IP地址
ifreq = (struct ifreq*)buf;  
for(i=(ifconf.ifc_len/sizeof(struct ifreq)); i>0; i--)
{
//      if(ifreq->ifr_flags == AF_INET){            //for ipv4
	printf("name =  %s\n", ifreq->ifr_name);
	printf("local addr = %s\n", inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr));
	ifreq++;

	if(strcmp(ifreq->ifr_name, WLAN_0) == 0)
	{
		printf(" wlan0  test ok! \n");
		return 1;
	}
//  }
}
return 0;
}
