/*************************************************************************
	> File Name: server.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2019年09月20日 星期五 17时22分39秒
 ************************************************************************/

#include<stdio.h>

/*server.c_阻塞式*/
/*
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
 
#define SIZE 512
 
int sock_bind(int lisfd, int port)
{
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
 
int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	int lisfd;
	int i;
	socklen_t len;
	char msg[SIZE];
	struct sockaddr_in cliaddr;
	
	bzero(msg, SIZE);
	len = sizeof(cliaddr);
	bzero(&cliaddr, sizeof(cliaddr));
	lisfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (lisfd == -1)
	{
		perror("socket failed.\n");
		exit(1);
	}
	sock_bind(lisfd, atoi(argv[1]));
	while(1)
	{
		recvfrom(lisfd, msg, SIZE, 0, (struct sockaddr *)&cliaddr, &len);
		printf("recv: %s\n", msg);
		for (i=0; i<strlen(msg); i++)//接收到的字符转换为大写的回送给客户端
		{
			msg[i] = toupper(msg[i]);
		}
		sendto(lisfd, msg, sizeof(msg), 0, (struct sockaddr *)&cliaddr, len);//udp发送要指定IP和端口
	}
	close(lisfd);
	return 0;
}

*/

/*server.c_非阻塞式*/
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
 
#define SIZE 512
 
int sock_bind(int lisfd, int port)
{
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
 
int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
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
		perror("socket failed.\n");
		exit(1);
	}
	sock_bind(lisfd, atoi(argv[1]));
	flag = fcntl(lisfd, F_GETFL, 0);
	if (flag < 0)
	{
		perror("fcntl failed.\n");
		exit(1);
	}
	flag |= O_NONBLOCK;
	if (fcntl(lisfd, F_SETFL, flag) < 0)
	{
		perror("fcntl failed.\n");
		exit(1);
	}
	while(1)
	{
		nbytes = recvfrom(lisfd, msg, SIZE, 0, (struct sockaddr *)&cliaddr, &len);
		if (nbytes == -1 && errno != EAGAIN)
		{
			perror("recv failed.\n");
			return 0;
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
		nbytes = sendto(lisfd, msg, sizeof(msg), 0, (struct sockaddr *)&cliaddr, len);//udp发送要指定IP和端口

		if (nbytes == -1 && errno != EAGAIN)
		{
			perror("recv failed.\n");
			exit(1);
		}
	}
	close(lisfd);
	return 0;
}

