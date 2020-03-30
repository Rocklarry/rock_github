#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#define OPEN_MAX 100

int main(int argc, char *argv[])
{
	struct epoll_event event;   // �����ں�Ҫ����ʲô�¼�  
    struct epoll_event wait_event; //�ں˼�����Ľ��
	
	//1.����tcp�����׽���
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	//2.��sockfd
	struct sockaddr_in my_addr;
	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(8001);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
	
	//3.����listen
	listen(sockfd, 10);
	 
	//4.epoll��Ӧ����׼��
	int fd[OPEN_MAX];
	int i = 0, maxi = 0;
	memset(fd,-1, sizeof(fd));
	fd[0] = sockfd;
	
	int epfd = epoll_create(10); // ����һ�� epoll �ľ��������Ҫ���� 0�� û��̫������  
    if( -1 == epfd ){  
        perror ("epoll_create");  
        return -1;  
    }  
      
    event.data.fd = sockfd;     //�����׽���  
    event.events = EPOLLIN; // ��ʾ��Ӧ���ļ����������Զ�
	
	//5.�¼�ע�ắ�����������׽��������� sockfd ��������¼�  
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);  
    if(-1 == ret){  
        perror("epoll_ctl");  
        return -1;  
    } 
	
	//6.�������ӵĿͻ��˵����ݴ���
	while(1)
	{
		// ���Ӳ��ȴ�����ļ�����׼���룬udp�׽��֣������������Ա仯���Ƿ�ɶ���  
        // û�����Ա仯�����������������ֱ���б仯������ִ�У�����û�����ó�ʱ   
        ret = epoll_wait(epfd, &wait_event, maxi+1, -1); 
		
		//6.1���sockfd(�����׽���)�Ƿ��������
		if(( sockfd == wait_event.data.fd )   
            && ( EPOLLIN == wait_event.events & EPOLLIN ) )
		{
			struct sockaddr_in cli_addr;
			int clilen = sizeof(cli_addr);
			
			//6.1.1 ��tcp�����������ȡ�ͻ���
			int connfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
			
			//6.1.2 ����ȡ����connfd����fd�����У��Ա�������ѯ�ͻ����׽���
			for(i=1; i<OPEN_MAX; i++)
			{
				if(fd[i] < 0)
				{
					fd[i] = connfd;
					event.data.fd = connfd; //�����׽���  
					event.events = EPOLLIN; // ��ʾ��Ӧ���ļ����������Զ�
					
					//6.1.3.�¼�ע�ắ�����������׽��������� connfd ��������¼�  
					ret = epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &event);  
					if(-1 == ret){  
						perror("epoll_ctl");  
						return -1;  
					} 
					
					break;
				}
			}
			
			//6.1.4 maxi����
			if(i > maxi)
				maxi = i;
				
			//6.1.5 ���û�о��������������ͼ���epoll��⣬����������¿�
			if(--ret <= 0)
				continue;
		}
		
		//6.2������Ӧ������������
		for(i=1; i<=maxi; i++)
		{
			if(fd[i] < 0)
				continue;
			
			if(( fd[i] == wait_event.data.fd )   
            && ( EPOLLIN == wait_event.events & (EPOLLIN|EPOLLERR) ))
			{
				int len = 0;
				char buf[128] = "";
				
				//6.2.1���ܿͻ�������
				if((len = recv(fd[i], buf, sizeof(buf), 0)) < 0)
				{
					if(errno == ECONNRESET)//tcp���ӳ�ʱ��RST
					{
						close(fd[i]);
						fd[i] = -1;
					}
					else
						perror("read error:");
				}
				else if(len == 0)//�ͻ��˹ر�����
				{
					close(fd[i]);
					fd[i] = -1;
				}
				else//�������յ�������������
					send(fd[i], buf, len, 0);
				
				//6.2.2���еľ����������������ˣ����˳���ǰ��forѭ��������poll���
				if(--ret <= 0)
					break;
			}
		}
	}
	return 0;
}