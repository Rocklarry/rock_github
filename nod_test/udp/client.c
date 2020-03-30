/*************************************************************************
	> File Name: client.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2019年09月20日 星期五 17时22分59秒
 ************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
 
#define SIZE 512
 
int main(int argc, char **argv)
{
	struct sockaddr_in remoaddr;
	struct sockaddr_in localaddr;
	if (argc != 3)
	{
		printf("Usage: %s <server_ipaddr> <port>\n", argv[0]);
		exit(1);
	}
	int sockfd;
	char msg[SIZE];
	socklen_t len;
	
	bzero(msg, SIZE);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	len = sizeof(localaddr);
	
	remoaddr.sin_family = AF_INET;
	remoaddr.sin_port = htons(atoi(argv[2]));
	
	if (inet_pton(AF_INET, argv[1], &remoaddr.sin_addr)<0)
	{
		perror("inet_pton failed.\n");
		exit(1);
	}
	
/*
	while(1)
	{
		char msg1[SIZE] ="reset resr *******  ====rock====\n" ;
		sendto(sockfd, msg1, SIZE, 0, (struct sockaddr *)&remoaddr, len);//udp发送要指定IP和端口
		recvfrom(sockfd, msg, SIZE, 0, (struct sockaddr *)&localaddr, &len);
		printf("recv: %s\n", msg);
		usleep(5000);
	}
*/

	while(fgets(msg, SIZE, stdin) != NULL)
	{
		sendto(sockfd, msg, SIZE, 0, (struct sockaddr *)&remoaddr, len);//udp发送要指定IP和端口
		recvfrom(sockfd, msg, SIZE, 0, (struct sockaddr *)&localaddr, &len);
		printf("recv: %s\n", msg);
	}
	close(sockfd);
	return 0;
}

