#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include <string.h> 
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
/* The name of the interface */
#ifndef IW_NAME
#define IW_NAME "eth1"
#endif

int main()
{
	int sockfd;
	struct iw_statistics stats;
	struct iwreq req;
	memset(&stats, 0, sizeof(stats));
	memset(&req, 0, sizeof(iwreq));
	sprintf(req.ifr_name, "wlan0");
	req.u.data.pointer = &stats;
	req.u.data.length = sizeof(iw_statistics);
	#ifdef CLEAR_UPDATED
	req.u.data.flags = 1;
	#endif

/* Any old socket will do, and a datagram socket is pretty cheap */
while(1)
{
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{
		perror("Could not create simple datagram socket");
		exit(EXIT_FAILURE);
	}

/* Perform the ioctl */
	if(ioctl(sockfd, SIOCGIWSTATS, &req) == -1) 
	{
		perror("Error performing SIOCGIWSTATS");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	
	close(sockfd);
	
		printf("Signal level%s is %d%s.\n",(stats.qual.updated & IW_QUAL_DBM ? " (in dBm)" :""),(signed char)stats.qual.level,
(stats.qual.updated & IW_QUAL_LEVEL_UPDATED ? " (updated)" :""));
		sleep(1);
	}
	return 0;
}
