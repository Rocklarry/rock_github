/*************************************************************************
	> File Name: netl.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2020年04月08日 星期三 11时23分37秒
 ************************************************************************/


#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/types.h>
#include <net/sock.h>
#include <net/netlink.h>
#define NETLINK_TEST 25 
#define MAX_MSGSIZE 1024
int stringlength(char *s);
int err;
struct sock *nl_sk = NULL;
int flag = 0;
//向用户态进程回发消息
void sendnlmsg(char *message, int pid)
{
    struct sk_buff *skb_1;
    struct nlmsghdr *nlh;
    int len = NLMSG_SPACE(MAX_MSGSIZE);
    int slen = 0;
    if(!message || !nl_sk)
    {
        return ;
    }
    printk(KERN_ERR "pid:%d\n",pid);
    skb_1 = nlmsg_new(len,GFP_KERNEL);
    if(!skb_1)
    {
        printk(KERN_ERR "my_net_link:alloc_skb error\n");
    }
    slen = stringlength(message);
    nlh = nlmsg_put(skb_1,0,0,0,MAX_MSGSIZE,0);
    NETLINK_CB(skb_1).portid = 0;
    NETLINK_CB(skb_1).dst_group = 0;
    //message[slen]= '\0';
    memcpy(nlmsg_data(nlh),message,slen);
    printk("my_net_link:send message '%s'.\n",(char *)nlmsg_data(nlh));
    netlink_unicast(nl_sk,skb_1,pid,MSG_DONTWAIT);
}

int stringlength(char *s)
{
    int slen = 0;
    for(; *s; s++)
    {
        slen++;
    }
    return slen;
}
//接收用户态发来的消息
void nl_data_ready(struct sk_buff *__skb)
 {
     struct sk_buff *skb;
     struct nlmsghdr *nlh;
     char str[100];
     struct completion cmpl;
     printk("begin data_ready\n");
     int i=10;
     int pid;
     skb = skb_get (__skb);
     if(skb->len >= NLMSG_SPACE(0))
     {
         nlh = nlmsg_hdr(skb);
         memcpy(str, NLMSG_DATA(nlh), sizeof(str));
         printk("Message received:%s\n",str) ;
         pid = nlh->nlmsg_pid;
         while(i--)
        {//我们使用completion做延时，每3秒钟向用户态回发一个消息
            init_completion(&cmpl);
            wait_for_completion_timeout(&cmpl,3 * HZ);
            sendnlmsg("I am from kernel!",pid);
        }
         flag = 1;
         kfree_skb(skb);
    }
 }

struct netlink_kernel_cfg netlink_cfg = {.input = nl_data_ready };
 
 
// Initialize netlink
int netlink_init(void)
{
	//memset(&netlink_cfg, 0, sizeof(struct netlink_kernel_cfg));//分配大小
    nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST,&netlink_cfg);// NETLINK_TEST获取 netli的协议
    if(!nl_sk){
        printk(KERN_ERR "my_net_link: create netlink socket error.\n");
        return 1;
    }
    printk("my_net_link_4: create netlink socket ok.\n");
    return 0;
}


static void netlink_exit(void)
{
    if(nl_sk != NULL){
        sock_release(nl_sk->sk_socket);
    }
    printk("my_net_link: self module exited\n");
}
module_init(netlink_init);
module_exit(netlink_exit);
MODULE_AUTHOR("xxxx");
MODULE_LICENSE("GPL");

