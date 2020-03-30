#include <stdio.h>  
#include <stdlib.h>  
#include <errno.h>
#include <memory.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <linux/netlink.h>  
  
#define MAX_MSGSIZE 512
#define NETLINK_TEST 25

int main(int argc, char * argv[])  
{  
    struct sockaddr_nl saddr, daddr;  
    struct nlmsghdr *nlhdr = NULL, *nlhdr_recv = NULL;  
    struct msghdr msg;  
    struct iovec iov;  
    int sock_fd, retval;
    int state_smg = 0;
    int ret = -1;  

    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
    if (-1 == sock_fd)
    {
        printf("error getting socket: %s\n", strerror(errno));
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

    strcpy(NLMSG_DATA(nlhdr),"Hello you!");

    iov.iov_base = (void *)nlhdr;
    iov.iov_len = NLMSG_SPACE(MAX_MSGSIZE);

    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&daddr;
    msg.msg_namelen = sizeof(daddr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    printf("state_smg\n");
    state_smg = sendmsg(sock_fd, &msg,0);
    if(-1 == state_smg)
    {
        printf("get error sendmsg = %s\n",strerror(errno));
    }

    memset(nlhdr, 0, NLMSG_SPACE(MAX_MSGSIZE));

    printf("waiting received!\n");
       
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

        printf("Received message: %s\n", NLMSG_DATA(nlhdr));
	}

   	close(sock_fd);
}  
