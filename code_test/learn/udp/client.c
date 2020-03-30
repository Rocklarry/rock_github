/*************************************************************************
	> File Name: client.c
	> Author: Rock
	> Mail: ruidongren@163.com 
	> Created Time: 2017年01月21日 星期六 15时47分46秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
	int cont;

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(50001);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //servaddr.sin_addr.s_addr = inet_addr("192.168.255.208");

    char sendline[100];
    sprintf(sendline, " Hello, word!\n");
	
	for(cont=0;cont< 10;cont++)
    sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

    close(sockfd);

    return 1;
}
