#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <assert.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <limits.h>

#define MAXLINE 4096
#define PORT 	2007
#define OPEN_MAX 1000

using namespace std;


void handler(int signo)
{
	int flag = 0;
	switch(signo)
	{
		case(SIGINT):
			close(PORT);
			flag =1;
			break;
		
		case(SIGQUIT):	
			close(PORT);
			flag =1;
			break;
		default :
			break;
	}

	if (flag ==1)
		exit(-1);
}

int main()
{
	struct sigaction sd;
	sigset_t oldmask,newmask;

	sigemptyset(&newmask);
	sigemptyset(&oldmask);

	struct sigaction sg;
	sg.sa_handler= handler;

	struct sockaddr_in sock,clisock;
	socklen_t clilen;
	sock.sin_family = AF_INET;
	sock.sin_port = htons(PORT);
	sock.sin_addr.s_addr = htonl(INADDR_ANY);
	
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	if(bind(fd, (sockaddr *)&sock, sizeof(sock))<0)
		cout<<"bind fd fail"<<endl;

	listen(fd, 10);
	
	int max = fd;
/*	int clients[FD_SETSIZE];

	for(int c= 0;c<FD_SETSIZE; c++)
		clients[c]=-1;*/

	struct pollfd clients[OPEN_MAX];
	for(auto &i :clients)
		i.fd =-1;
	clients[0].fd = fd;
	clients[0].events = POLLRDNORM;
	
	int max_cli = -1;

	int newfd,i,n,nready = 0;
	char buf[MAXLINE];

	cout<<getpid()<<endl;

	for(;;)
	{
		nready  = poll(clients, OPEN_MAX, -1);
	
		if(clients[0].revents & POLLRDNORM)
		{
			newfd = accept(fd, reinterpret_cast<sockaddr*>(&clisock), &clilen);
			for(i =1; i<OPEN_MAX ; ++i)
			{
				if(clients[i].fd <0)
				{
					clients[i].fd = newfd;
					clients[i].events = POLLRDNORM;
					break;
				}
			}
			if(i == OPEN_MAX)
				cout<<"too many clients"<<endl;
			if(i> max_cli)
				max_cli = i;

			if(--nready <= 0)
				continue;	
		}

		for(i =1; i<= max_cli; ++i)
		{
			if(clients[i].fd <0)
				continue;
			if(clients[i].revents & (POLLRDNORM | POLLERR))
			{
				if( (n =read(clients[i].fd, buf, MAXLINE))<0)
					cout<<"read error"<<endl;
				else if(n ==0)
				{
					cout<<"a connection close"<<endl;
					close(clients[i].fd);
					clients[i].fd = -1;
				}
				else{
					int len = 0;
					while(len < n)
						len += write(clients[i].fd, &buf[len], n-len);
				}
			if(--nready <= 0)
				break;
			}
		}
	}
	return 0;
}