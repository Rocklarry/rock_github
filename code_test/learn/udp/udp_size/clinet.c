/*************************************************************************
	> File Name: clinet.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2018年10月18日 星期四 15时40分37秒
 ************************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define BUFSIZE 512

int main(int argc, char* argv[])

{

    int sfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(sfd == -1)
    {
        perror("socket");
        exit(-1);
    }

 

    struct sockaddr_in toaddr;
    bzero(&toaddr, sizeof(toaddr));
    toaddr.sin_family = AF_INET;
    toaddr.sin_port = htons(atoi(argv[2]));
    toaddr.sin_addr.s_addr = inet_addr(argv[1]);
    sendto(sfd, "hello", 6, 0, (struct sockaddr*)&toaddr, sizeof(struct sockaddr));

 

    char buf[BUFSIZE] = {0};
    struct sockaddr_in fromaddr;
    bzero(&fromaddr, sizeof(fromaddr));
    int fromaddrlen = sizeof(struct sockaddr);
    int filelen = 0;   //用于保存文件长度

    FILE* fp = fopen("./2.txt","w+b");

//接收文件的长度

	recvfrom(sfd, (void*)&filelen, sizeof(int), 0, (struct sockaddr*)&fromaddr, &fromaddrlen);
    printf("the length of file is %d\n",filelen);
    printf("Create a new file!\n");
    printf("begin to reveive file content!\n");

    //接收文件的内容

while(1)

    {
        int len = recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr*)&fromaddr, &fromaddrlen);
        if(len < BUFSIZE)
//如果接收的长度小于BUFSIZE，则表示最后一次接收，此时要用break退出循环

        {
            fwrite(buf,sizeof(char),len,fp);
            break;
        }
        fwrite(buf,sizeof(char),len,fp);
    }

    printf("receive file finished!\n");

    close(sfd);

}
