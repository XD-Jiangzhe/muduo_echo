#include <iostream>
#include <string>
#include <vector>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/epoll.h>


#define MAXLINE 4096
#define PORT 	2007


using namespace std;

void handler(int);

class sign{
	public:
		sign();
};

sign sig;
int fd;
char buf[MAXLINE];
int main()
{

	struct sockaddr_in sock,clisock;
	socklen_t clilen = static_cast<socklen_t>(sizeof(sockaddr_in));
	sock.sin_family = AF_INET;
	sock.sin_port = htons(PORT);
	sock.sin_addr.s_addr = htonl(INADDR_ANY);
	
	fd = socket(AF_INET, SOCK_STREAM, 0);

	if(bind(fd, (sockaddr *)&sock, sizeof(sock))<0)
		cout<<"bind fd fail"<<endl;


	listen(fd, 10);

	cout<<getpid()<<endl;

	int  efd = epoll_create1(EPOLL_CLOEXEC);
	if(efd == -1)
		cout<<"create efd fail"<<endl;

	//创建一个监听套接字的epoll_event结构体
	struct epoll_event listen;
	listen.events = EPOLLIN;
	listen.data.fd = fd;
	epoll_ctl(efd, EPOLL_CTL_ADD, fd, &listen);

	struct epoll_event clieve;
	
	//用来返回活跃epoll_event结构体的数组
/*	好像不可以用数组来操作
	vector<struct epoll_event> events;
	
*/

	int left_fd = open("/dev/null", O_RDONLY|O_CLOEXEC);
	struct epoll_event eventss[MAXLINE];

/*循环遍历所有的活跃的套接字*/
	for(; ;)
	{
		int nready = epoll_wait(efd, eventss, MAXLINE /*&*events.begin(), int(events.size())*/ , -1);

/*		if(nready == events.size())
			events.resize(nready*2);*/

		//寻找是否listen套接字可读,如果可读，取出建立连接的套接字，然后将其放进监听队列中
		for(int i=0; i<nready; ++i)
		{
			int clifd;
			if(eventss[i].data.fd == fd)
			{
				clifd = accept4(fd, (struct sockaddr*)(&clisock), &clilen, SOCK_NONBLOCK|SOCK_CLOEXEC);
			

				if(clifd == -1)
				{
					if(errno == EMFILE)
					{
						close(left_fd);
						clifd = accept4(fd, (struct sockaddr*)(&clisock), &clilen, SOCK_NONBLOCK|SOCK_CLOEXEC);
						
						close(clifd);
						left_fd = open("/dev/null", O_RDONLY|O_CLOEXEC); 
					}
				}

				clieve.data.fd = clifd;
				clieve.events = EPOLLIN;

				epoll_ctl(efd, EPOLL_CTL_ADD, clifd, &clieve);						//放入进去关注
				
				

				if(--nready <=0)
					break;
			}

			bzero(buf, MAXLINE);
			int n =  read(eventss[i].data.fd, buf, MAXLINE);
			int len = 0;
			if(n >0)
			{	
				while(len<n)
				{
					len += write(eventss[i].data.fd, &buf[len], n-len);
				}
			}
			else if(n == 0)
			{
				shutdown(eventss[i].data.fd, SHUT_RD);
				
				epoll_ctl(efd, EPOLL_CTL_DEL, eventss[i].data.fd, &eventss[i]);


				if(--nready <=0)
					break;
			}
		}


		
	}

	return 0;
}


inline sign::sign(){
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT,	handler);
	signal(SIGQUIT, handler);
}

void handler(int signo)
{
	int flag = 0;
	switch(signo)
	{
		case(SIGINT):
			close(fd);
			flag =1;
			break;
		
		case(SIGQUIT):	
			close(fd);
			flag =1;
			break;
		default :
			break;
	}

	if (flag ==1)
		exit(-1);
}