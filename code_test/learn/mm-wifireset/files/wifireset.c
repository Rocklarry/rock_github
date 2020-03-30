#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <memory.h>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <linux/netlink.h>  
#include <fcntl.h>  
#include <sys/select.h>  
#include<sys/ioctl.h>
#include <syslog.h>
#include <unistd.h>
  
#define MAX_MSGSIZE 512
#define NETLINK_TEST 25


int wifi_check(void)  
{  
    struct sockaddr_nl saddr, daddr;  
    struct nlmsghdr *nlhdr = NULL;// *nlhdr_recv = NULL;  
    struct msghdr msg;  
    struct iovec iov;  
    int sock_fd, retval;
    int state_smg = 0;
    int ret = -1;
	static int coun = 0;
	 
    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
    if (-1 == sock_fd)
    {
        printf("error getting socket: %s\n", strerror(errno));
		return -1;
    }

    memset(&saddr, 0, sizeof(saddr));  
    memset(&daddr, 0, sizeof(daddr));  
    saddr.nl_family = AF_NETLINK;        
    saddr.nl_pid = getpid();  
    saddr.nl_groups = 0;
    

    retval = bind(sock_fd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (retval < 0)
    {
        printf("bind failed: %s\n", strerror(errno));
        close(sock_fd);
        return -1;
    }

    nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_MSGSIZE));
    if (NULL == nlhdr)
    {
        printf("malloc nlmsghdr error!\n");
        close(sock_fd);
        return -1;
    }

    daddr.nl_family = AF_NETLINK;
    daddr.nl_pad = 0;
    daddr.nl_groups = 0;

    nlhdr->nlmsg_len = NLMSG_SPACE(MAX_MSGSIZE);
    nlhdr->nlmsg_pid = getpid();
    nlhdr->nlmsg_flags = 0;

    strcpy((char *)NLMSG_DATA(nlhdr),"Hello you!");

    iov.iov_base = (void *)nlhdr;
    iov.iov_len = NLMSG_SPACE(MAX_MSGSIZE);

    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&daddr;
    msg.msg_namelen = sizeof(daddr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

   // printf("state_smg\n");
    state_smg = sendmsg(sock_fd, &msg,0);
    if(-1 == state_smg)
    {
        printf("get error sendmsg = %s\n",strerror(errno));
		return 0;
    }

    memset(nlhdr, 0, NLMSG_SPACE(MAX_MSGSIZE));

    printf("waiting received!\n");
	syslog(LOG_INFO," waiting received!\n");

    while (1)
    {
        printf("In while recvmsg\n");
   	 	memset(nlhdr, 0, NLMSG_SPACE(MAX_MSGSIZE));
    	ret = recvmsg(sock_fd, &msg, 0);  
		
        if (ret < 0) 
        {
            perror("recvmsg:");
            exit(1);
        }
		 coun++;
        printf(" coun = %d  Received message: %s \n ",coun, (char *)NLMSG_DATA(nlhdr));
		syslog(LOG_INFO," coun = %d  Received message: %s \n ",coun, (char *)NLMSG_DATA(nlhdr));

		//if (strcmp((char *)NLMSG_DATA(nlhdr), "FW_CRASH=1") == 0)
		if (memcmp((char *)NLMSG_DATA(nlhdr), "FW_CRASH=1",strlen("FW_CRASH=1")) == 0)
			break;
		usleep(5000);
	}

   	close(sock_fd);

	printf(" exit sock \n");

	return 1;

}  

void wifi_rest_cmd(void)
{
		usleep(50);
		system("rmmod /lib/modules/3.4.0-eagle8074/kernel/drivers/net/wireless/wlan.ko");
		syslog(LOG_INFO,"wlan0 rmmod");
		usleep(50);
		system("insmod /lib/modules/3.4.0-eagle8074/kernel/drivers/net/wireless/wlan.ko");
		//system("pkill hostapd");
		//system("hostapd /etc/hostapd.conf");
		usleep(500);
		system("ifdown wlan0");
		usleep(500);
		system("ifup wlan0");
		usleep(50);
}


int  main(int argc, char * argv[])  
{

	syslog(LOG_INFO,"\t\t*setusbcomposition mtp \n");
	execl("/bin/sh", "sh", "/usr/bin/setusbcomposition", "mtp", NULL);

	usleep(50);
	printf("wifi test  \n");

	while(1)
	{
		if (wifi_check())
		{
		wifi_rest_cmd();
		syslog(LOG_INFO,"wifi  rest  ok!");
		}
	usleep(5000);
	}
	return 0;
}
