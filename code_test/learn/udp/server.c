/*************************************************************************
	> File Name: service.c
	> Author: Rock
	> Mail: ruidongren@163.com 
	> Created Time: 2017年01月21日 星期六 15时46分00秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <conio.h>

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(50001);

    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	
	while(1){
    int n;
    char recvline[1024];

    recvfrom(sockfd, recvline, 1024, 0, NULL, NULL);

    printf("%s", recvline);

	/*if(getch()=='\n')
		break;*/
	}

    close(sockfd);
	return 0;
}
